/********************************************************************
  This code was developed entirely by Supratik Chakraborty at IIT Bombay, 
  India.  No part of this code may be used for commercial or other 
  purposes without explicit permission from supratik@cse.iitb.ac.in

  Last updated: May 25, 2013
*********************************************************************/
// Using the following #define is not recommended.  However, the
// WSTE tool requires this.
#define SWAP_HI_LO_IN_SELWITHINT

#include <iostream>
#include <cassert>
#include <exception>
#include "../include/Oprs.h"


using namespace std;
BVManager * BVManager::bvmanager_instance = NULL;
BVManager::BVManager() { /* constructor */
  if (bvmanager_instance != NULL) {
    cerr << "Error in BVManager constructor: Attempt to create multiple copies of BVManager" << endl;
    cerr << "There should be only one copy of BVManager" << endl;
    assert(0);
  }

  block_size = ((unsigned long) sizeof(bv_block_t) * BYTE_SIZE);
  avail_block_size = block_size - 1;
  cumulative_array_size = 0;
  /*allocate an array of so many (block_size) bv_block_t elements and 
    store the array in mask.*/
  try {
    mask = new bv_block_t[block_size];
  }
  catch (bad_alloc&) {
    cerr << "Memory allocation failure in BVManager constructor." << endl;
    assert(0);
  }

  try {
    cumulative_mask = new bv_block_t[block_size];
  }
  catch (bad_alloc&) {
    cerr << "Memory allocation failure in BVManager constructor." << endl;
    assert(0);
  }
  
  bv_block_t cumulative_mask_entry = 0;
  for (unsigned long i = 0; i < block_size; i++) {
    mask[i] = ((bv_block_t) 1) << i;
    cumulative_mask_entry = ((cumulative_mask_entry << 1) | ((bv_block_t) 1));
    cumulative_mask[i] = cumulative_mask_entry;
  }
  
  width_of_max_representable_int = 
    (unsigned long) (sizeof(unsigned long) * BYTE_SIZE);
  
  hex_width = 4;
  if ((block_size % hex_width) != 0) {
    cerr << "ERROR in BVManager constructor: Block size " << block_size;
    cerr << " not a multiple of hex_width " << hex_width << endl;
    assert(0);
  }

  hex_blocks_in_block = block_size/hex_width;
  try {
    hex_blocks_mask = new bv_block_t[hex_blocks_in_block];
  }
  catch (bad_alloc&) {
    cerr << "Memory allocation error in BVManager constructor." << endl;
    assert(0);
  }

  for (unsigned long i = 0; i < hex_blocks_in_block; i++) {
    hex_blocks_mask[i] = ((bv_block_t) 0);
    for (unsigned long j = 0; j < hex_width; j++) {
      hex_blocks_mask[i] = (hex_blocks_mask[i] | mask[i*hex_width + j]);
    }
  }
}

BVManager::~BVManager() { /* destructor */
  delete[] mask;
  delete[] hex_blocks_mask;

  list<bv_block_t *>::iterator i;
  for (i = list_of_bv_blocks.begin(); i != list_of_bv_blocks.end(); i++) {
    if ((*i) != NULL) {
      delete[] (*i);
    }
  }
}

BVManager* BVManager::getInstance()
{
  if(bvmanager_instance == NULL) {
    try {
      bvmanager_instance = new BVManager();
    }
    catch(bad_alloc&) {
      cerr << "Memory allocation failure in BVManager::getInstance" << endl;
      assert(0);
    }
  }
  return bvmanager_instance;
}

inline unsigned long max(unsigned long i1,unsigned long i2)
{
  return (i1<i2)?i2:i1;
}

string BVManager::printBVasBinString(bvatom bv_inst)
{
  char *bvstr;
  try {
    bvstr = new char[bv_inst.bv_width + 1];
  }
  catch (bad_alloc&) {
    cerr << "Memory allocation failure in BVManager::printBVasBinString" << endl;
    assert(0);
  }

  unsigned long bv_block = bv_inst.bv_array_size - 1;
  unsigned long bv_offset = offset_from_pos(bv_inst.bv_width - 1);
  unsigned long bv_str_index = 0;

  for ( ; ; bv_block--) {
    for ( ; ; bv_offset--) {
      // An unsigned long is always >= 0, therefore it is meaningless to check
      // bv_block >= 0 or bv_offset >= 0 in the condition parts of the for
      // statements.
      
      if ((bv_inst.bv_array[bv_block] & mask[bv_offset]) != ((bv_block_t) 0)) {
        bvstr[bv_str_index]='1';
      }
      else {
        bvstr[bv_str_index]='0';
      }
      bv_str_index++;

      if (bv_offset == 0) break;
    }

    if (bv_block == 0) {
      break;
    }
    else {
      bv_offset = avail_block_size - 1;
    }
  }

  assert(bv_str_index == bv_inst.bv_width);
  bvstr[bv_inst.bv_width]='\0';
  string result(bvstr);
  delete[] bvstr;
  return result;     
}

void BVManager::printBVatom(bvatom bv_inst, int pretty)
{
  if (pretty == 1) {
    printPrettyBVatom(bv_inst);
  }
  else if (pretty == 0) {
    printHexBV(bv_inst.bv_array, bv_inst.bv_array_size * block_size);
    cout << "[" << dec << bv_inst.bv_width << "]";
  }
  else if (pretty == -1) {
    string result = printBVasBinString(bv_inst);
    cout << result;
    cout << "[" << dec << bv_inst.bv_width << "]";
    result.erase();
  }
}

void BVManager::printPrettyBVatom(bvatom bv_inst)
{
  unsigned long trim_array_size = bv_inst.bv_width/block_size;

  if ((bv_inst.bv_width % block_size) != 0) {
    trim_array_size++;
  }

  bv_block_t *trim_bv;

  try {
    trim_bv = new bv_block_t[trim_array_size];
  }
  catch (bad_alloc&) {
    cerr << "Memory allocation failure in BVManager::printPrettyBVatom." << endl;
    assert(0);
  }

  for (unsigned long i = 0; i < trim_array_size; i++) {
    trim_bv[i] = ((bv_block_t) 0);
  }
  
  for (unsigned long i = 0; i < bv_inst.bv_width; i++) {
    unsigned long trim_block = i/block_size;
    unsigned long trim_offset = i % block_size;

    unsigned long bv_block = i/avail_block_size;
    unsigned long bv_offset = i % avail_block_size;

    if ((bv_inst.bv_array[bv_block] & mask[bv_offset]) != ((bv_block_t) 0)) {
      trim_bv[trim_block] = (trim_bv[trim_block] | mask[trim_offset]);
    }
  }

  printHexBV(trim_bv, bv_inst.bv_width);
  cout << "[" << dec << bv_inst.bv_width << "]";

  delete[] trim_bv;
}

void BVManager::printHexBV(bv_block_t *bvarray, unsigned long width)
{
  if (width == 0) {
    cerr << "ERROR in BVManager::printHexBV: Can't print bitvector of length 0." << endl;
    assert(0);
  }

  if ((width % hex_width) != 0) {
    width += (hex_width - (width % hex_width));
    /* This makes width a multiple of hex_width */
  }
    

  unsigned long hex_blocks_mask_index;
  if ((width % block_size) == 0) {
    hex_blocks_mask_index = hex_blocks_in_block - 1;
  }
  else {
    hex_blocks_mask_index = (width % block_size)/hex_width - 1;
  }

  cout << " 0x";
  for (unsigned long i = width-1; ; i = i - hex_width) {
    // An unsigned long is always >= 0, therefore it is meaningless to check
    // i >= 0 in the condition part of the for statement.

    unsigned long block = i / block_size;
    bv_block_t curr_val = (bvarray[block] & hex_blocks_mask[hex_blocks_mask_index]);
    if (hex_blocks_mask_index != 0) {
      curr_val = curr_val >> (hex_blocks_mask_index * hex_width);
      hex_blocks_mask_index--; // this decrement doesn't lead to any wrap-around
                               // problem for unsigned long, because we have 
                               // already checked that hex_blocks_mask_index != 0
    }
    else {
      hex_blocks_mask_index = (block_size/hex_width - 1);
    }

    cout << hex << curr_val;

    if (i < hex_width) break;
  }
}

unsigned long BVManager:: get_bv_array_size(unsigned long w)
{
 unsigned long result = w/ (avail_block_size);
 if (result * avail_block_size < w) {
   result = result + 1;
 }
 return result;
}

bvatom BVManager::getZeroedBVatom(unsigned long width) 
{
  bvatom result;
  result.bv_array_size = get_bv_array_size(width);
  result.bv_width = width;
  // allocate an array of so many (result.bv_array_size) bv_block_t elements and
  //  store in result.bv_array;					

  try {
    result.bv_array= new bv_block_t[result.bv_array_size];
  }
  catch (bad_alloc&) {
    cerr << "Memory allocation failure in BVManager::getZeroedBVatom." << endl;
    assert(0);
  }

  //  initialize all elements of array to (bv_block_t) 0.
  for(unsigned long i = 0; i < result.bv_array_size; i++) {
    result.bv_array[i] = ((bv_block_t) 0);
  }

  cumulative_array_size += result.bv_array_size;
  list_of_bv_blocks.push_back(result.bv_array);
  list<bv_block_t *>::iterator it = list_of_bv_blocks.end();
  result.list_of_bv_blocks_iterator = --it;

  // No need to cleanup the result, since we just initialized it to 0
  return result;
}

void BVManager::replaceBySelBVatom(bvatom bv_src, bvatom &bv_dest, unsigned long src_start, unsigned long dest_start, unsigned long length) 
{

  cleanup(bv_dest);
  cleanup(bv_src);

  if (bv_dest.bv_width < dest_start + length) {
    cerr << "ERROR in BVManager::replaceBySelBVatom: Insufficient width in destination BV ..."<<endl;
    assert(0);
  }

  if (bv_src.bv_width < src_start + length) {
   cerr << "ERROR in BVManager::replaceBySelBVatom: Insufficient width in source BV ..."<<endl;
   assert(0);
 }

 /* Three cases to consider */

 if (dest_start == src_start) {
   unsigned long src_start_pos = offset_from_pos(src_start);
   unsigned long src_start_block = block_from_pos(src_start);
   unsigned long src_end_pos = offset_from_pos(src_start + length - 1);
   unsigned long src_end_block = block_from_pos(src_start + length-1);

   bv_block_t start_mask = ((bv_block_t) 0);
   bv_block_t end_mask = ((bv_block_t) 0);

   for (unsigned long i = 0; i < src_start_pos; i++) {
     start_mask = (start_mask | mask[i]);
   }

   for (unsigned long i = 0; i <= src_end_pos; i++) {
     end_mask = (end_mask | mask[i]);
   }

   if (src_start_block == src_end_block) {
     end_mask = (~start_mask & end_mask);
     start_mask = ~end_mask;
   }
 
   bv_block_t curr_val = bv_dest.bv_array[src_start_block];
   curr_val = (curr_val & start_mask);
   curr_val = (curr_val | (bv_src.bv_array[src_start_block] & ~start_mask));
   bv_dest.bv_array[src_start_block] = curr_val;

   for (unsigned long i = src_start_block + 1; i < src_end_block; i++) {
     bv_dest.bv_array[i] = bv_src.bv_array[i];
   }

   curr_val = bv_dest.bv_array[src_end_block];
   curr_val = (curr_val & ~end_mask);
   curr_val = (curr_val | (bv_src.bv_array[src_end_block] & end_mask));
   bv_dest.bv_array[src_end_block] = curr_val;
   
 }
 else if (src_start > dest_start) {
   unsigned long shift = src_start - dest_start;
   
   /* Preserve width while right shifting */
   bvatom temp_bv = rightshiftBVatom(bv_src, shift, 1);
   replaceBySelBVatom(temp_bv, bv_dest, dest_start, dest_start, length);
   freeBVatom(temp_bv);
 }
 else { /* dest_start > src_start */
   unsigned long shift = dest_start - src_start;

   /* Extend width while left shifting */
   bvatom temp_bv = leftshiftBVatom(bv_src, shift, 0);
   replaceBySelBVatom(temp_bv, bv_dest, dest_start, dest_start, length);
   freeBVatom(temp_bv);
 }
}

bvatom BVManager::getBVatom(unsigned long width, string &bv_str)
{
  if (width == 0) {
    cerr << "ERROR in BVManager::getBVatom: getBVatom invoked with zero width" << endl;
    assert(0);
  }

  bvatom result = getZeroedBVatom(width);
  unsigned long last_block_offset, curr_block, bv_str_index;

  if (width <= bv_str.size()) {
    /* A long binary string is to be read in a short bit-vector.
       Read only the relevant least signficant bits of the binary string. */

    last_block_offset = offset_from_width(width); /* width % avail_block_size; */
    curr_block = block_from_width(width); /* This equals result.bv_array_size - 1; */
    assert(curr_block == (result.bv_array_size - 1));
    bv_str_index = bv_str.size() - width; /* Use only the last width bits in bv_str */
  }
  else {
    /* width > bv_str.width() */
    /* A short binary string is to be read in a long bit-vector.
       Pad 0's in the most significant bits. */

    last_block_offset = offset_from_width(bv_str.size());
    curr_block = block_from_width(bv_str.size());
    bv_str_index = 0;
  }
  
  int done = 0;
  if (last_block_offset < avail_block_size - 1) {
    
    /* fill in result.bv_array[result.bv_array_size-1] with the first
       (leftmost) last_block_offset characters of bv_str */
    
    bv_block_t curr_val = ((bv_block_t) 0);
    
    for (unsigned long i = last_block_offset; ; i--) { 
      // An unsigned long is always >= 0, therefore it is meaningless to check
      // i >= 0 in the condition part of the for statement.
      curr_val = curr_val << 1;
      if ((bv_str[bv_str_index] != '0') && (bv_str[bv_str_index] != '1')) {
	cerr << "ERROR in BVManager::getBVatom: Invalid char " << bv_str[bv_str_index];
	cerr << " of string " << bv_str << " at index " << bv_str_index << endl;
	cerr << "Expected char: 0 or 1" << endl;
	assert(0);
      }
      else {
	if (bv_str[bv_str_index] == '1') {
	  curr_val |= ((bv_block_t) 1);
	}
	bv_str_index++;
      }

      if (i == 0) break;
    }
    result.bv_array[curr_block] = curr_val;

    if (curr_block == 0) {
      done = 1;
    }
    else {
      curr_block--;
    }
  }
  
  if (done == 0) {
    /* done == 0 implies curr_block > 0 */

    for (; ; curr_block--) {
      // An unsigned long is always >= 0, therefore it is meaningless to check
      // curr_block >= 0 in the condition part of the for statement.
      
      bv_block_t curr_val = ((bv_block_t) 0);
      
      for (unsigned long i = avail_block_size - 1; ; i--) {
	// An unsigned long is always >= 0, therefore it is meaningless to check
	// i >= 0 in the condition part of the for statement.

	curr_val = curr_val << 1;
	if ((bv_str[bv_str_index] != '0') && (bv_str[bv_str_index] != '1')) {
	  cerr << "ERROR in BVManager::getBVatom: Invalid char of string " << bv_str;
	  cerr << " at index " << bv_str_index << " : " << bv_str[bv_str_index] << endl;
	  cerr << "Expected char: 0 or 1" << endl;
	  assert(0);
	}
	else {
	  if (bv_str[bv_str_index] == '1') {
	    curr_val |= ((bv_block_t) 1);
	  }
	  bv_str_index++;
	}
	
	if (i == 0) break;
      }
      
      result.bv_array[curr_block] = curr_val;
      if (curr_block == 0) break;
    }
    
   
  }
  
  if (bv_str_index != bv_str.size()) {
    cerr << "ERROR in BVManager::getBVatom: bv string not of same width as specified." << endl;
    assert(0);
  }

  cleanup(result);
  return result;
}

void BVManager::freeBVatom(bvatom bv_inst) 
{
 cumulative_array_size -= bv_inst.bv_array_size;
 list_of_bv_blocks.erase(bv_inst.list_of_bv_blocks_iterator);
 delete[] bv_inst.bv_array;
}

unsigned long BVManager::convertBVToUnsignedInt(bvatom bv)
{
  unsigned long curr_block = bv.bv_array_size - 1;
  unsigned long bit_index_offset = 0;

  while (true) {
    // An unsigned long is always >= 0, therefore it is meaningless to check
    // curr_block >= 0 in the condition part of the while statement.

    if (bv.bv_array[curr_block] == ((bv_block_t) 0)) {
      if (curr_block == 0) {
	break;
      }
      else {
	curr_block--;
      }
    }
    else {
      /* curr_block has at least one 1.
	 We now need to check if the there is actually 1 at
	 a bit position that exceeds width_of_max_representable_int */

      bit_index_offset = curr_block * avail_block_size;
      for (unsigned long i = avail_block_size - 1; ; i--) {
	// An unsigned long is always >= 0, therefore it is meaningless to check
	// i >= 0 in the condition part of the for statement.

	if ((bv.bv_array[curr_block] & mask[i]) != ((bv_block_t) 0)) {
	  if ((bit_index_offset + i + 1) > width_of_max_representable_int) {
	    cerr<<"ERROR in BVManager::ConvertBVToUnsignedInt: Bitvector with 1 at index ";
	    cerr << bit_index_offset + i + 1 << " cannot be represented on machine ";
	    cerr << "with unsigned long represented by " << sizeof(unsigned long) << " bytes" << endl;
	    assert(0);
	  }
	  else {
	    break;
	  }
	}
	if (i == 0) break;
      }
      break;
    }
  }
  
  unsigned long result = (unsigned long) 0;

  for(unsigned long i = bv.bv_array_size - 1; ; i--) {
    // An unsigned long is always >= 0, therefore it is meaningless to check
    // i >= 0 in the condition part of the for statement.

    result=(result<<(avail_block_size)) + (unsigned long) bv.bv_array[i];

    if (i == 0) break;
  }

  return result;
}

bvatom BVManager::left_shift(bvatom bv_inst, bvatom bv_shift)
{
  unsigned long shift=convertBVToUnsignedInt(bv_shift);
  /* Last parameter = 1 indicates width of bitvector to be preserved */
  bvatom result = leftshiftBVatom(bv_inst, shift, 1);

  // leftshiftBVatom will return a cleaned up result, so no need to
  // cleanup again
  return result;
}

bvatom BVManager::ext_left_shift(bvatom bv_inst, bvatom bv_shift)
{
  unsigned long shift=convertBVToUnsignedInt(bv_shift);
  /* Last parameter = 0 indicates width of bitvector to be extended, if needed */
  bvatom result = leftshiftBVatom(bv_inst, shift, (unsigned long) 0);

  // leftshiftBVatom will return a cleaned up result, so no need to
  // cleanup again
  return result;
}

bvatom BVManager::right_shift(bvatom bv_inst, bvatom bv_shift)
{
  unsigned long shift=convertBVToUnsignedInt(bv_shift);
  /* Last parameter = 1 indicates width of bitvector to be preserved */
  bvatom result = rightshiftBVatom(bv_inst, shift, 1);

  // rightshiftBVatom will return a cleaned up result, so no need to
  // cleanup again
  return result;
}

bvatom BVManager::leftshiftBVatom(bvatom bv_inst, unsigned long shift, int preserve_width)
{
  unsigned long width;
  if (preserve_width == 0) {
    width = ((unsigned long) bv_inst.bv_width) + shift;
  }
  else {
    width = (unsigned long) bv_inst.bv_width;
  }

  unsigned long untruncated_width = bv_inst.bv_width + shift;
  unsigned long untruncated_arr_size = get_bv_array_size(untruncated_width);

  bvatom result = getZeroedBVatom(width);
  
  unsigned long start_block = block_from_pos(shift);
  unsigned long start_pos = offset_from_pos(shift);

  bv_block_t start_mask = ((bv_block_t) 0);

  for (unsigned long i = 0; i < avail_block_size - start_pos; i++) {
    start_mask = (start_mask | mask[i]);
  }

  bv_block_t residue = ((bv_block_t) 0);
  for (unsigned long i = start_block; i < result.bv_array_size; i++) {
    bv_block_t curr_val;

    assert(i-start_block <= bv_inst.bv_array_size);

    if (i-start_block < bv_inst.bv_array_size) {
      curr_val = (bv_inst.bv_array[i - start_block] & start_mask);
      curr_val = curr_val << start_pos;
      curr_val = (curr_val | residue);
      residue = (bv_inst.bv_array[i - start_block] & ~start_mask);
      residue = residue >> (avail_block_size - start_pos);
    }
    else {
      curr_val = residue;
      residue = ((bv_block_t) 0);
      assert (i == (result.bv_array_size - 1));
    }
  
    result.bv_array[i] = curr_val;
  }    

  cleanup(result);
  return result;
}

bvatom BVManager::rightshiftBVatom(bvatom bv_inst, unsigned long shift, int preserve_width)
{
  unsigned long width;
  bvatom result;

  if (preserve_width == 0) {
    width = bv_inst.bv_width + shift;
    result = getZeroedBVatom(width);

    /* Simply pad "shift" 0's to the left of bv_inst */
    for (unsigned long i = 0; i < bv_inst.bv_array_size; i++) {
      result.bv_array[i] = bv_inst.bv_array[i];
    }
  }
  else {
    width = bv_inst.bv_width;
    result = getZeroedBVatom(width);
  
    unsigned long start_block = block_from_pos(shift);
    unsigned long start_pos = offset_from_pos(shift);
    
    bv_block_t start_mask = ((bv_block_t) 0);
  
    for (unsigned long i = start_pos; i < avail_block_size; i++) {
      start_mask = (start_mask | mask[i]);
    }
    
    bv_block_t residue;
    for (unsigned long i = start_block; i < bv_inst.bv_array_size; i++) {
      bv_block_t curr_val;
      
      if (i != (bv_inst.bv_array_size - 1)) {
	curr_val = (bv_inst.bv_array[i] & start_mask);
	curr_val = curr_val >> start_pos;
	residue = (bv_inst.bv_array[i+1] & ~start_mask);
	residue = residue << (avail_block_size - start_pos);
	curr_val = (curr_val | residue);
      }
      else {
	curr_val = (bv_inst.bv_array[i] & start_mask);
	curr_val = curr_val >> start_pos;
      }

      result.bv_array[i - start_block] = curr_val;
    }    
  }
 
  cleanup(result);
  return result;
}

bvatom BVManager::BVExtend(bvatom bv_inst, unsigned long width, int sign_extend)
{

  if(width <= bv_inst.bv_width)
    {
      /* We don't truncate bv_inst, instead return the whole of bv_inst */

      bvatom result=getZeroedBVatom(bv_inst.bv_width);
      for (unsigned long i = 0; i < bv_inst.bv_array_size; i++) {
	result.bv_array[i] = bv_inst.bv_array[i];
      }

      cleanup(result);
      return result;
    }
  else
    {
      unsigned long msb_offset = get_msb_offset(bv_inst);	
      unsigned long msb;

      if ((sign_extend == 1) && isMsb1(bv_inst))
	{
	  msb = 1;
	}
      else
	{
	  msb = 0;
	}

      bvatom result = getZeroedBVatom(width);

      for (unsigned long i = 0; i < bv_inst.bv_array_size; i++) {
	result.bv_array[i] = bv_inst.bv_array[i];
      }

      if(msb == 1)
	{
	  bv_block_t start_mask = ((bv_block_t) 0);

	  for (unsigned long i = msb_offset + 1; i < avail_block_size; i++) {
	    start_mask = (start_mask | mask[i]);
	  }

	  bv_block_t mid_mask = ~mask[block_size - 1];

	  unsigned long end_offset = offset_from_width(width);
	  bv_block_t end_mask = ((bv_block_t) 0);
	  for (unsigned long i = 0; i <= end_offset; i++) {
	    end_mask = (end_mask | mask[i]);
	  }

	  if (result.bv_array_size == bv_inst.bv_array_size) {
	    bv_block_t curr_val = result.bv_array[result.bv_array_size - 1];
	    result.bv_array[result.bv_array_size - 1] = ((curr_val | start_mask) & end_mask);
	  }
	  else {
	    result.bv_array[bv_inst.bv_array_size - 1] =
	      (result.bv_array[bv_inst.bv_array_size - 1] | start_mask);

	    for (unsigned long i = bv_inst.bv_array_size; i < result.bv_array_size - 1; i++) {
	      result.bv_array[i] = mid_mask;
	    }

	    result.bv_array[result.bv_array_size - 1] = end_mask;
	  }
	}

      // This is already a cleaned up result; so no need to cleanup again.
      return result;
    }
}


bvatom BVManager::arith_neg(bvatom bv_inst)
{
  if(checkBVForZero(bv_inst)) {
      return getZeroedBVatom(bv_inst.bv_width);
  }
  else {
    bvatom result = getZeroedBVatom(bv_inst.bv_width);
    int flag = 0;
    for (unsigned long i = 0; i < bv_inst.bv_array_size; i++) {
      if(flag == 1) {
	bv_block_t current_val = (~(bv_inst.bv_array[i])) & (~(mask[block_size-1]));
	result.bv_array[i] = current_val;
      }
      else {
	if (bv_inst.bv_array[i] == ((bv_block_t) 0)) {
	  result.bv_array[i] = ((bv_block_t) 0);
	}
	else {
	  bv_block_t current_val = (bv_inst.bv_array[i]);
	  unsigned long counter = 0;
	  bv_block_t current_mask = mask[0];
	  while(counter < avail_block_size) {
	    if((current_val % 2) == 1) {
	      break;
	    }
	    else {
	      counter++;
	      current_val = (current_val >> 1);
	      current_mask = (current_mask | mask[counter]);
	    }
	  }
	  
	  if (counter == avail_block_size) {
	    cerr<<"ERROR in BVManager::arith_neg: Shift overflow" << endl;
	    assert(0);
	  }
	  
	  current_val = (bv_inst.bv_array[i] & current_mask);
	  bv_block_t higher_current_val = (bv_inst.bv_array[i] & ~current_mask);
	  higher_current_val =
	    ((~higher_current_val & ~current_mask) & ~mask[block_size-1]);
	  result.bv_array[i] = (current_val | higher_current_val);
	  
	  flag=1;
	}
      }
    }//end for

    /* Now ensure that proper width is restored to the result bitvector */
    
    unsigned long width = bv_inst.bv_width;
    unsigned long msb_block = block_from_width(width);
    unsigned long msb_offset = offset_from_width(width);
    
    bv_block_t current_mask = ((bv_block_t) 0);
    for(unsigned long i = 0; i <= msb_offset; i++) {
      current_mask = (current_mask | mask[i]);
    }
    result.bv_array[msb_block] = (result.bv_array[msb_block] & current_mask);
    
    cleanup(result);
    return result;
  }
}

bvatom BVManager::arith_signed_add(bvatom bv_inst1_in, bvatom bv_inst2_in)
{
  unsigned long w1 = bv_inst1_in.bv_width;
  unsigned long w2 = bv_inst2_in.bv_width;
  bvatom bv_inst1, bv_inst2;

  if (w1 > w2) {
    bv_inst2=BVExtend(bv_inst2_in, w1, 1);
    bv_inst1=bv_inst1_in;
  }
  else if (w1 < w2) {
    bv_inst1=BVExtend(bv_inst1_in, w2, 1);
    bv_inst2=bv_inst2_in;
  }
  else {
    bv_inst1=bv_inst1_in;
    bv_inst2=bv_inst2_in;
  }

  unsigned long msb1 = isMsb1(bv_inst1) ? 1:0;
  unsigned long msb2 = isMsb1(bv_inst2) ? 1:0;

  bvatom result = arith_add(bv_inst1, bv_inst2);
  unsigned long maxw = max(bv_inst1.bv_width, bv_inst2.bv_width);

  unsigned long msbr = isMsb1(result) ? 1:0;

  if ((msb1 == 1) && (msb2 == 1)) {
    /* Both operands were negative. Result must have an additional bit,
       and result.bv_width = max of operands' widths + 1. msbr must be
       1 in this case.
       However, if both msbr and the previous bit of the result are 1,
       one of the 1's can be shaved off.
    */
    
    unsigned long m2sbr_offset = offset_from_width(result.bv_width - 1);
    unsigned long m2sbr_block = block_from_width(result.bv_width - 1);

    unsigned long m2sbr = 
      ((result.bv_array[m2sbr_block] & mask[m2sbr_offset]) == ((bv_block_t) 0)) ? 0:1;

    if (m2sbr == 1) {
      bvatom new_result = getZeroedBVatom(result.bv_width - 1);
      replaceBySelBVatom(result, new_result, (unsigned long) 0, (unsigned long) 0, result.bv_width - 1);
      freeBVatom(result);
      result = new_result;
    }
  }
  else if ((msb1 == 0) && (msb2 == 0)) {
    /* Both operands were positive. Result cannot have an additional bit,
       and result.bv_width = max of operands' widths. 
       Now, if msbr is 1, a 0 has to be padded to the left, so that the
       result is not interpretted as negative.
    */

    if (msbr == 1) {
      bvatom new_result = BVExtend(result, result.bv_width + 1, 0);
      freeBVatom(result);
      result = new_result;
    }
  }
  else {
    /* One operand is positive and the other negative.  The width of
       the result need not be larger than the max width of the operands.
    */

    if (result.bv_width > maxw) {
      bvatom new_result = getZeroedBVatom(maxw);
      replaceBySelBVatom(result, new_result, (unsigned long) 0, (unsigned long) 0, maxw);
      freeBVatom(result);
      result = new_result;
    }
  }
  
  cleanup(result);
  return result;
}

bvatom BVManager::arith_add(bvatom bv_inst1_in, bvatom bv_inst2_in)
{
  /* Assume both operands are unsigned. */

  unsigned long w1 = bv_inst1_in.bv_width;
  unsigned long w2 = bv_inst2_in.bv_width;
  bvatom bv_inst1,bv_inst2;

  if(w1 > w2)
    {
      bv_inst2=BVExtend(bv_inst2_in, w1, 0);
      bv_inst1=bv_inst1_in;
    }
  else if(w1 < w2)
    {
      bv_inst1=BVExtend(bv_inst1_in, w2, 0);
      bv_inst2=bv_inst2_in;
      
    }
  else
    {
      bv_inst1=bv_inst1_in;
      bv_inst2=bv_inst2_in;
    }


  unsigned long width = bv_inst1.bv_width+ 1;
  bvatom result = getZeroedBVatom(width);

  unsigned long max_arr_sz = bv_inst1.bv_array_size;

  unsigned long carry = (unsigned long) 0; 
  bv_block_t curr_result = ((bv_block_t) 0);

  for (unsigned long i = 0; i < max_arr_sz; i++) { 
    curr_result = (bv_inst1.bv_array[i]) + (bv_inst2.bv_array[i]) + ((bv_block_t) carry);
    unsigned long carry_out = 
      ((curr_result & mask[block_size -1]) == ((bv_block_t) 0)) ? ((bv_block_t) 0): ((bv_block_t) 1);
    curr_result = (curr_result & ~mask[block_size-1]);
    result.bv_array[i] = curr_result;
    carry = carry_out;
  }//end for

  if (carry != ((unsigned long) 0)) {
    if (max_arr_sz < result.bv_array_size) {
      result.bv_array[result.bv_array_size - 1] = ((bv_block_t) 1);
    }
  }

  /* Check if it is ok to reduce the bit vector width by 1 */

  if (!isMsb1(result))
    {
      //ok to decrease width
      bvatom new_result=getZeroedBVatom(width - 1);
      replaceBySelBVatom(result, new_result, (unsigned long) 0, (unsigned long) 0, width-1);
      freeBVatom(result);
      result = new_result;
    }
    
  if(w1 > w2)
    {
      freeBVatom(bv_inst2);
    }
  else if(w1 < w2)
    {
      freeBVatom(bv_inst1);
    }
  
  cleanup(result);
  return(result);
}

bvatom BVManager::arith_sub(bvatom bv_inst1, bvatom bv_inst2)
{
  /* Both arguments assumed to be signed bitvectors */

  bvatom neg_bv_inst2 = arith_neg(bv_inst2);

  if (checkBVEquality(neg_bv_inst2, bv_inst2) && (!checkBVForZero(bv_inst2))) {
    /* There are only two values whose 2's compl. negation has the same
       representation as the original no.
       One of these is 10000000...0000 and the other is 0.
       In the former case, when we negate the value, we need to
       pad a 0 at the left to designate the positive value.
       Otherwise, neg(1000) would again be 1000, i.e.
       neg(-8) would be -8 again.  With an additional 0 padded
       to the left of the result, we get neg(1000) = 01000,
       or neg(-8) = 8.
    */
    neg_bv_inst2 = BVExtend(bv_inst2, bv_inst2.bv_width + 1, 0);
  }

  bvatom result=arith_signed_add(bv_inst1, neg_bv_inst2);
  
  // arith_signed_add already returns a cleaned up result, so need to
  // cleanup again.
  return result;
}

bvatom BVManager::arith_mult(bvatom bv_inst1_in, bvatom bv_inst2_in)
{
  /* Assume both operands are unsigned */

  unsigned long w1 = bv_inst1_in.bv_width;
  unsigned long w2 = bv_inst2_in.bv_width;

  unsigned long width = w1+w2;
  bvatom result = getZeroedBVatom(width);
  bvatom shifted_op = getZeroedBVatom(width);

  /* Let bv_inst1 be the shorter bit vector, and bv_inst2 be the longer one */
  
  bvatom bv_inst1, bv_inst2;

  if (w1 <= w2) {
    bv_inst1 = bv_inst1_in;
    bv_inst2 = bv_inst2_in;
  }
  else {
    bv_inst1 = bv_inst2_in;
    bv_inst2 = bv_inst1_in;
  }

  unsigned long incremental_shift;

  /* Copy bv_inst2 into shifted_op */
  replaceBySelBVatom(bv_inst2, shifted_op, (unsigned long) 0, (unsigned long) 0, bv_inst2.bv_width);
  incremental_shift = 0; // Added by Supratik

  for (unsigned long i = 0; i < bv_inst1.bv_array_size; i++) {
    bv_block_t curr_val = bv_inst1.bv_array[i];

    if (curr_val == ((bv_block_t) 0)) {
      /* Width preserving left shift by avail_block_size */
      // bvatom temp_shifted_op = leftshiftBVatom(shifted_op, avail_block_size, 1);  Commented by Supratik
      bvatom temp_shifted_op = leftshiftBVatom(shifted_op, avail_block_size + incremental_shift, 1); // Added by Supratik
      freeBVatom(shifted_op);
      shifted_op = temp_shifted_op;
      incremental_shift = 0; // Added by Supratik
    }
    else {
      // incremental_shift = 0;  Removed by Supratik
      unsigned long max_offset = 0;

      if (i == (bv_inst1.bv_array_size - 1)) {
	max_offset = get_msb_offset(bv_inst1) + 1;
      }
      else {
	max_offset = avail_block_size;
      }

      for (unsigned long j = 0; j < max_offset; j++) {
	if ((bv_inst1.bv_array[i] & mask[j]) != ((bv_block_t) 0)) {
	  if (incremental_shift > 0) { 
	    bvatom temp_shifted_op = leftshiftBVatom(shifted_op, incremental_shift, 1);
	    freeBVatom(shifted_op);
	    shifted_op = temp_shifted_op;
	    incremental_shift = 0; // Added by Supratik
	  }
	  
	  bvatom temp_result = arith_add(result, shifted_op);
	  freeBVatom(result);
	  result = temp_result;
	  
	  // incremental_shift = 0; Commented by Supratik
	}

	incremental_shift++;
      }
    }
  }

  cleanup(result);

  /* Now check if the width really needs to be larger than the
     longest operand's, i.e., bv_inst2's bit width */

  bvatom truncated_result = getZeroedBVatom(bv_inst2.bv_width);
  // truncated_result is of course cleaned up

  replaceBySelBVatom(result, truncated_result, (unsigned long) 0, (unsigned long) 0, bv_inst2.bv_width);
  // replaceBySelBVatom ensures that truncated_result is cleaned up

  bvatom ext_trunc_result = BVExtend(truncated_result, result.bv_width, 0);
  // BVExtend returns a cleaned up result

  if (checkBVEquality(result, ext_trunc_result)) {
    freeBVatom(result);
    freeBVatom(ext_trunc_result);
    return truncated_result;
  }
  else {
    freeBVatom(truncated_result);
    freeBVatom(ext_trunc_result);
    return result;
  }
}

bvatom BVManager::arith_div_mod(bvatom dividend, bvatom divisor, int div_mod_flag)
{
  cleanup(dividend);
  cleanup(divisor);

  unsigned long w1 = dividend.bv_width; 
  unsigned long w2 = divisor.bv_width; 

  /* Assume both dividend and divisor are unsigned, and extend them 
     to the same bit width */

  bvatom ext_dividend, ext_divisor;

  if (w1 < w2) {
    if (isMsb1(divisor)) {
      ext_dividend = BVExtend(dividend, w2+1, 0);
      ext_divisor = BVExtend(divisor, w2+1, 0);
      // BVExtend returns cleaned up bvatoms
    }
    else {
      ext_dividend = BVExtend(dividend, w2, 0);
      // BVExtend returns cleaned up bvatoms
      ext_divisor = divisor;
    }
  }
  else if (w2 < w1) {
    if (isMsb1(dividend)) {
      ext_divisor = BVExtend(divisor, w1+1, 0);
      ext_dividend = BVExtend(dividend, w1+1, 0);
      // BVExtend returns cleaned up bvatoms
    }
    else {
      ext_divisor = BVExtend(divisor, w1, 0);
      // BVExtend returns cleaned up bvatoms
      ext_dividend = dividend;
    }      
  }
  else { /* w1 == w2 */
    if (isMsb1(dividend) || isMsb1(divisor)) {
      ext_dividend = BVExtend(dividend, w1+1, 0);
      ext_divisor = BVExtend(divisor, w1+1, 0);
      // BVExtend returns cleaned up bvatoms 
    }
    else {
      ext_dividend = dividend;
      ext_divisor = divisor;
    }
  }

  /* Now ext_dividend.bv_width == ext_divisor.bv_width and each
     has a 0 as its msb.  This 0-padding is needed to allow the
     unsigned long integer interpretation even when we use arith_sub
     below.
  */
 
  bvatom quotient = getZeroedBVatom((unsigned long) 1);
  bvatom residue;

  if (checkBVForZero(ext_divisor)) {
    cerr << "ERROR in BVManager::arith_div_mod: Division invoked with zero divisor.\n";
    assert(0);
  }
  else if (checkBVGreaterThan(ext_divisor, ext_dividend)) {
    /* Divisor greater than dividend */

    quotient.bv_array[0] = ((bv_block_t) 0);
    residue = getZeroedBVatom(ext_dividend.bv_width);
    replaceBySelBVatom(ext_dividend, residue, (unsigned long) 0, (unsigned long) 0, ext_dividend.bv_width);
    // replaceBySelBVatom ensures residue is cleaned up
    
    /* Ready to return after garbage collection */
  }
  else if (checkBVEquality(ext_divisor, ext_dividend)) {
    /* Divisor equal to dividend */

    quotient.bv_array[0] = ((bv_block_t) 1);
    residue = getZeroedBVatom(ext_dividend.bv_width);

    /* Ready to return after garbage collection */
  }
  else {
    /* Divisor less than dividend */

    unsigned long true_divisor_width = get_len_ignoring_left_zero_padding(ext_divisor);
    
    bvatom const_one = getZeroedBVatom((unsigned long) 1);
    const_one.bv_array[0] = ((bv_block_t) 1);
    bvatom const_two = getZeroedBVatom((unsigned long) 2);
    const_two.bv_array[0] = ((bv_block_t) 2);
    
    quotient.bv_array[0] = ((bv_block_t) 0);
    /* Copy ext_dividend in residue */

    residue = getZeroedBVatom(ext_dividend.bv_width);
    // residue is of course cleaned up

    replaceBySelBVatom(ext_dividend, residue, (unsigned long) 0, (unsigned long) 0, ext_dividend.bv_width);
    // replaceBySelBVatom ensures reside is cleaned up

    do {
      /* Loop invariant: 
	 residue > ext_divisor  AND
	 residue.bv_width == ext_divisor.bv_width */

      bvatom new_residue = arith_sub(residue, ext_divisor);
      // arith_sub returns a cleaned up bvatom
      
      if (checkBVGreaterThan(ext_divisor, new_residue)) {
	bvatom new_quotient = arith_add(quotient, const_one);
	// arith_add returns a cleaned up bvatom

	freeBVatom(quotient);
	freeBVatom(residue);
	quotient = new_quotient;
	residue = new_residue;
	break;
	/* Ready to return after garbage collecting */
      }
      else if (checkBVEquality(ext_divisor, new_residue)) {
	bvatom new_quotient = arith_add(quotient, const_two);
	// arith_add returns a cleaned up bvatom

	freeBVatom(quotient);
	freeBVatom(residue);
	quotient = new_quotient;
	residue = getZeroedBVatom((unsigned long) 1);
	residue.bv_array[0] = ((bv_block_t) 0);
	break;
	/* Ready to return after garbage collecting */
      }
      else {
	/* new_residue > ext_divisor, i.e. residue > 2*ext_divisor */
	
	freeBVatom(new_residue);
	
	unsigned long true_residue_width = get_len_ignoring_left_zero_padding(residue);
	unsigned long width_diff = true_residue_width - true_divisor_width;
	
	/* Width preserving left shift of divisor. Recall that the total
	   bit widths (as in bvatom class) of residue and ext_divisor 
	   are already matched up.
	*/
	bvatom shifted_divisor = leftshiftBVatom(ext_divisor, width_diff, 1);
	// leftshiftBVatom returns a cleaned up bvatom

	if (checkBVGreaterThan(shifted_divisor, residue)) {
	  /* Left shift by one less.
	     Since we already know that new_residue = residue - divisor > divisor,
	     therefore residue > 2*divisor.
	     Therefore, width_diff must be > 1.  And hence, width_diff - 1 >= 1.
	  */
	  width_diff --;
	  assert(width_diff >= 1);
	  freeBVatom(shifted_divisor);
	  shifted_divisor = leftshiftBVatom(ext_divisor, width_diff, 1);
	  // leftshiftBVatom returns a cleaned up bvatom
	}
	
	new_residue = arith_sub(residue, shifted_divisor);
	bvatom increment_quotient = leftshiftBVatom(const_one, width_diff, 0);
	bvatom new_quotient = arith_add(quotient, increment_quotient);
	// All three above operations return cleaned up bvatoms

	freeBVatom(increment_quotient);
	freeBVatom(quotient);
	freeBVatom(residue);
	freeBVatom(shifted_divisor);

	residue = new_residue;
	quotient = new_quotient;
	
	if (checkBVForZero(residue) || checkBVGreaterThan(ext_divisor, residue)) {
	  break;
	  
	  /* Ready to return after garbage collection */
	}
	else if (checkBVEquality(residue, ext_divisor)) {
	  bvatom new_quotient = arith_add(quotient, const_one);
	  // arith_add returns a cleaned up bvatom
	  freeBVatom(quotient);
	  freeBVatom(residue);
	  quotient = new_quotient;
	  residue = getZeroedBVatom((unsigned long) 1);
	  residue.bv_array[0] = ((bv_block_t) 0);
	  break;
	}
      }
    } while (1);
    
    freeBVatom(const_one);
    freeBVatom(const_two);
  }

  /* Garbage collection and return */
  if (ext_dividend.bv_width != dividend.bv_width) {
    freeBVatom(ext_dividend);
  }
  if (ext_divisor.bv_width != divisor.bv_width) {
    freeBVatom(ext_divisor);
  }

  if (div_mod_flag == 1) {
    freeBVatom(residue);
  
    /* Trim down the quotient of residue keeping it an unsigned bit vector */
    unsigned long quot_len = get_len_ignoring_left_zero_padding(quotient);
    if (quot_len == 0) {
      freeBVatom(quotient);
      quotient = getZeroedBVatom(1);
      // quotient is of course cleaned up
      quotient.bv_array[0] = ((bv_block_t) 0);
    }
    else {
      if (quot_len != quotient.bv_width) {
	bvatom new_quotient = getZeroedBVatom(quot_len);
	replaceBySelBVatom(quotient, new_quotient, (unsigned long) 0, (unsigned long) 0, quot_len);
	// replaceBySelBVatom ensures new_quotient is cleaned up
      
	freeBVatom(quotient);
	quotient = new_quotient;
      }
    }
    // At this point, quotient is cleaned up
    return quotient;
  }
  else {
    freeBVatom(quotient);

    /* Trim down the width of residue keeping it an unsigned bit vector */
    unsigned long res_width = get_len_ignoring_left_zero_padding(residue);
    if (res_width == 0) {
      freeBVatom(residue);
      residue = getZeroedBVatom(1);
      // residue is of course cleaned up

      residue.bv_array[0] = ((bv_block_t) 0);
    }
    else {
      if (res_width != residue.bv_width) {
	bvatom new_res = getZeroedBVatom(res_width);
	// new_res is of course cleaned up

	replaceBySelBVatom(residue, new_res, (unsigned long) 0, (unsigned long) 0, res_width);
	// replaceBySelBVatom ensures new_res is cleaned up

	freeBVatom(residue);
	residue = new_res;
      }
    }
    
    // At this point, residue is already cleaned up
    return residue;
  }
}

unsigned long BVManager::get_len_ignoring_left_zero_padding(bvatom bv_inst)
{
  cleanup(bv_inst);
  if (checkBVForZero(bv_inst)) {
    return 0;
  }

  int found_leftmost_block = 0;
  unsigned long leftmost1_block;
  unsigned long i;
  for (i = bv_inst.bv_array_size - 1; ; i--) {
    // An unsigned long is always >= 0, therefore it is meaningless to check
    // i >= 0 in the condition part of the for statement.

    if (bv_inst.bv_array[i] != ((bv_block_t) 0)) {
      leftmost1_block = i;
      found_leftmost_block = 1;
      break;
    }
    
    if (i == 0) break;
  }

  if (found_leftmost_block == 0) {
    cerr << "ERROR in BVManager::get_len_ignoring_left_zero_padding: Could not find leftmost 1 when checking length.\n";
    assert(0);
  }
  else {
    int found_leftmost_offset = 0;
    unsigned long leftmost1_offset;
  
    for (unsigned long j = avail_block_size - 1; ; j--) {
      // An unsigned long is always >= 0, therefore it is meaningless to check
      // j >= 0 in the condition part of the for statement.

      if ((bv_inst.bv_array[i] & mask[j]) != ((bv_block_t) 0)) {
	leftmost1_offset = j;
	found_leftmost_offset = 1;
	break;
      }

      if (j == 0) {
	break;
      }
    }
    
    if (found_leftmost_offset == 0) {
      cerr << "ERROR in BVManager::get_len_ignoring_left_zero_padding: Could not find leftmost 1 when checking length.\n";
      assert(0);
    }
      
    return (unsigned long) (leftmost1_block*avail_block_size + leftmost1_offset + 1);
  }
}

bvatom BVManager::arith_div(bvatom dividend, bvatom divisor)
{
  return arith_div_mod(dividend, divisor, 1);
  // arith_div_mod returns a cleaned up bvatom
}

bvatom BVManager::arith_mod(bvatom dividend, bvatom divisor)
{
  return arith_div_mod(dividend, divisor, 0);
  // arith_div_mod returns a cleaned up bvatom
}

bvatom BVManager::bitwise_and(bvatom bv_inst1,bvatom bv_inst2)
{

  if (bv_inst1.bv_width != bv_inst2.bv_width) {
    cerr<<"ERROR in BVManager::bitwise_and: Unequal widths of arguments of bitwise and" << endl;
    assert(0);
  }
  else {
    bvatom result = getZeroedBVatom(bv_inst1.bv_width);
    bv_block_t msbmask = ~mask[block_size-1];
    for(unsigned long i = 0; i < bv_inst1.bv_array_size; i++) {
      result.bv_array[i] = ((bv_inst1.bv_array[i] &  bv_inst2.bv_array[i]) & msbmask); 
    }
    
    cleanup(result);
    return result;
  }
}


bvatom BVManager::bitwise_or(bvatom bv_inst1,bvatom bv_inst2)
{

  if (bv_inst1.bv_width != bv_inst2.bv_width) {
    cerr<<"ERROR in BVManager::bitwise_or: Unequal widths of arguments of bitwise or" << endl;
    assert(0);
  }
  else {
    bvatom result = getZeroedBVatom(bv_inst1.bv_width);
    bv_block_t msbmask = ~mask[block_size-1];
    for(unsigned long i = 0; i < bv_inst1.bv_array_size; i++) {
      result.bv_array[i] = ((bv_inst1.bv_array[i] |  bv_inst2.bv_array[i]) & msbmask); 
    }
    cleanup(result);
    return result;
  }
}


bvatom BVManager::bitwise_xor(bvatom bv_inst1,bvatom bv_inst2)
{

  if(bv_inst1.bv_width != bv_inst2.bv_width) {
    cerr<<"ERROR in BVManager::bitwise_xor: Unequal widths of arguments of bitwise xor" << endl;
    assert(0);
  }
  else {
    bvatom result = getZeroedBVatom(bv_inst1.bv_width);
    bv_block_t msbmask = ~mask[block_size-1];
    for(unsigned long i = 0; i < bv_inst1.bv_array_size; i++) {
      result.bv_array[i] = ((bv_inst1.bv_array[i] ^  bv_inst2.bv_array[i]) & msbmask); 
    }
    cleanup(result);
    return result;
  }
}


bvatom BVManager::bitwise_xnor(bvatom bv_inst1,bvatom bv_inst2)
{
  if (bv_inst1.bv_width != bv_inst2.bv_width) {
    cerr<<"ERROR in BVManager::bitwise_xnor: Unequal widths of arguments of bitwise xnor" << endl;
    assert(0);
  }
  else {
    bvatom result = getZeroedBVatom(bv_inst1.bv_width);
    bv_block_t spcl_bit_mask = ~mask[block_size-1];
    for(unsigned long i = 0; i < bv_inst1.bv_array_size; i++) {
      result.bv_array[i] = (~(bv_inst1.bv_array[i] ^  bv_inst2.bv_array[i]) & spcl_bit_mask); 
    }
    
    unsigned long last_offset = get_msb_offset(result); /* result.bv_width % avail_block_size; */
    if(last_offset < (avail_block_size - 1)) {
      bv_block_t last_mask = ((bv_block_t) 0);
      for (unsigned long i = last_offset + 1; i < block_size; i++) {
	last_mask = (last_mask | mask[i]);
      }
      last_mask = ~last_mask;
      result.bv_array[result.bv_array_size -1 ] = (result.bv_array[result.bv_array_size -1] & last_mask);
    }
    
    cleanup(result);
    return result;
  }
}
  
  
bvatom BVManager::bitwise_negate(bvatom bv_inst1)
{

  bvatom result = getZeroedBVatom(bv_inst1.bv_width);
  bv_block_t spcl_bit_mask = ~mask[block_size-1];
  for(unsigned long i = 0; i < bv_inst1.bv_array_size; i++) {
    result.bv_array[i] = (~(bv_inst1.bv_array[i]) & spcl_bit_mask); 
  }
  unsigned long last_offset = get_msb_offset(result);
  if (last_offset < avail_block_size - 1) {
    bv_block_t last_mask = ((bv_block_t) 0);
    for(unsigned long i = last_offset+1; i<block_size; i++) {
      last_mask = (last_mask | mask[i]);
    }
    last_mask = ~last_mask;
    result.bv_array[result.bv_array_size -1 ] = (result.bv_array[result.bv_array_size -1 ] & last_mask);
  }
  
  cleanup(result);
  return result;
}

bvatom BVManager::reduction_and(bvatom bv_inst1)
{
  cleanup(bv_inst1);

  bvatom result = getZeroedBVatom((unsigned long) 1);
  bv_block_t spcl_bit_mask = ~mask[block_size-1];
  int flag = 0;
  for(unsigned long i = 0; i < bv_inst1.bv_array_size - 1; i++) {
    bv_block_t current = (~(bv_inst1.bv_array[i] ) & spcl_bit_mask);
    if (current != ((bv_block_t) 0)) {
      flag = 1;
      break;
    }
  }
  if (flag == 1) {
    return result;
  }
  else {
    unsigned long last_offset = get_msb_offset(bv_inst1);
    if (last_offset < avail_block_size - 1) {
      bv_block_t last_mask = ((bv_block_t) 0);
      for (unsigned long i = last_offset + 1; i < block_size; i++) {
	last_mask = (last_mask | mask[i]);
      }
      last_mask = ~last_mask;
      if ((~(bv_inst1.bv_array[result.bv_array_size-1]) & last_mask) != 
	  ((bv_block_t) 0)) {
	return result; /* i.e. 0 */
      }
      else {
	result.bv_array[0] = ((bv_block_t) 1);
	return result;
      }
    }
    else { /* last_offset == avail_block_size - 1 */
      
      if ((~(bv_inst1.bv_array[bv_inst1.bv_array_size - 1]) & spcl_bit_mask) ==
	  ((bv_block_t) 0)) {
	result.bv_array[0] = ((bv_block_t) 1);
      }
      else {
	/* result = 0 */
      }
      return result;
    }
  }
}


bvatom BVManager::reduction_or(bvatom bv_inst1)
{
  cleanup(bv_inst1);

  bvatom result = getZeroedBVatom((unsigned long) 1);
  bv_block_t spcl_bit_mask = ~mask[block_size-1];
  int flag = 0;
  for(unsigned long i = 0; i < bv_inst1.bv_array_size; i++) {
    bv_block_t current = ((bv_inst1.bv_array[i]) & spcl_bit_mask);
    if (current != ((bv_block_t) 0)) {
      flag = 1;
      break;
    }
  }
  if (flag == 0) {//note carefully
    return result;
  }
  else {
    result.bv_array[0] = ((bv_block_t) 1);
    return result;
  }
}

bvatom BVManager::reduction_nor(bvatom bv_inst1)
{
  // bv_inst1 will be cleaned up in reduction_or 
  bvatom result = reduction_or(bv_inst1);
  if (result.bv_array[0] == ((bv_block_t) 0)) {
    result.bv_array[0] = ((bv_block_t) 1);
  }
  else {
    result.bv_array[0] = ((bv_block_t) 0);
  }

  return result;
}

bvatom BVManager::reduction_nand(bvatom bv_inst1)
{
  // bv_inst1 will be cleaned up in reduction_and
  bvatom result = reduction_and(bv_inst1);
  if (result.bv_array[0] == ((bv_block_t) 0)) {
    result.bv_array[0] = ((bv_block_t) 1);
  }
  else {
    result.bv_array[0] = ((bv_block_t) 0);
  }

  return result;
}

bvatom BVManager::reduction_xor(bvatom bv_inst1)
{
  cleanup(bv_inst1);

  bvatom result = getZeroedBVatom((unsigned long) 1);
  unsigned long count_of_ones = 0;
  for(unsigned long i = 0; i < bv_inst1.bv_array_size; i++) {
    bv_block_t current = bv_inst1.bv_array[i];
    for(unsigned long j = 0; j < avail_block_size; j++) {
      if((current % 2) == 1) {
	count_of_ones++;
      }
      current = (current>>1);
    }
  }
  if ((count_of_ones % 2) == 1) {
    result.bv_array[0]= ((bv_block_t) 1);
  }

  return result;
}

bvatom BVManager::reduction_xnor(bvatom bv_inst1)
{
  // bv_inst1 will be cleaned up in reduction_xor
  bvatom result = reduction_xor(bv_inst1);
  if (result.bv_array[0] == ((bv_block_t) 0)) {
    result.bv_array[0] = ((bv_block_t) 1);
  }
  else {
    result.bv_array[0] = ((bv_block_t) 0);
  }

  return result;
}  


bvatom BVManager::concat(bvatom bv_inst1,bvatom bv_inst2)
{
  unsigned long w1=bv_inst1.bv_width;
  unsigned long w2=bv_inst2.bv_width;

  bvatom result = BVExtend(bv_inst2, w1+w2, 0);
  // BVExtend returns a cleaned up bvatom

  replaceBySelBVatom(bv_inst1, result, (unsigned long) 0, w2, w1);
  // replaceBySelBVatom ensures result is cleaned up
  
  return result;
}


bvatom BVManager::select_with_int(bvatom bv_inst1, unsigned long hi, unsigned long lo)
{
  if (hi < lo) {
#ifndef SWAP_HI_LO_IN_SELWITHINT
    cerr << "ERROR in BVManager::select_with_int: hi (" << hi << ") of select_with_int < lo (" << lo << ")" << endl;
    assert(0);
#else
    cerr << "Swapping! hi (" << hi << ") of select_with_bv < lo (" << lo << ")" << endl;
    unsigned long temp = hi;
    hi = lo;
    lo = temp;
#endif
  }

  bvatom result=getZeroedBVatom((unsigned long) (hi-lo+1));
  // result is of course cleaned up

  replaceBySelBVatom(bv_inst1, result, lo, (unsigned long) 0, (unsigned long) (hi-lo+1));
  // replaceBySelBVatom ensures result stays cleaned up

  return result;

}

bvatom BVManager::select_with_bv(bvatom bv_inst1, bvatom bv_hi, bvatom bv_lo)
{
  unsigned long hi=convertBVToUnsignedInt(bv_hi);
  unsigned long lo=convertBVToUnsignedInt(bv_lo);
  if (hi < lo) {
#ifndef SWAP_HI_LO_IN_SELWITHINT
    cerr << "ERROR in BVManager::select_with_bv: hi (" << hi << ") of select_with_bv < lo (" << lo << ")" << endl;
    assert(0);
#else
    cerr << "Swapping! hi (" << hi << ") of select_with_bv < lo (" << lo << ")" << endl;
    unsigned long temp = hi;
    hi = lo;
    lo = temp;
#endif
  }
  bvatom result = select_with_int(bv_inst1, hi, lo);
  // select_with_int returns a cleaned up bvatom
  return result;
}

bvatom BVManager::reverseBV(bvatom bv_inst)
{
  bvatom result = getZeroedBVatom(bv_inst.bv_width);
  
  if (checkBVForZero(bv_inst)) {
    /* An easy case */
    return(result);
  }

  unsigned long src_pos = bv_inst.bv_width - 1;
  unsigned long src_block = block_from_pos(src_pos);
  bv_block_t src_chunk = bv_inst.bv_array[src_block];
  bv_block_t src_mask = mask[offset_from_pos(src_pos)];
  
  unsigned long dest_block = 0;
  bv_block_t dest_mask = mask[0];
  bv_block_t result_chunk = ((bv_block_t) 0);
  bv_block_t dest_mask_overflow = (((bv_block_t) 1) << block_size);
  
  for (src_pos++; src_pos > 0; src_pos--) {
    /* Since src_pos is unsigned long, it will never take a negative
       value.  So we increment it to bv_inst.bv_width at the 
       beginning of the for loop and then iterate until src_pos
       becomes 0.
    */
    
    if ((src_chunk & src_mask) != ((bv_block_t) 0)) {
      result_chunk |= dest_mask;
    }
    
    src_mask = src_mask >> 1;
    if (src_mask == ((bv_block_t) 0)) { /* underflow, so wrap around */
      src_mask = mask[block_size - 2];
      // we use block_size - 2 because the most significant bit of the
      // word is reserved for the carry bit.
      if (src_block > 0) {
	src_block--;
	src_chunk = bv_inst.bv_array[src_block];
      }
    }

    dest_mask = dest_mask << 1;
    if (dest_mask == dest_mask_overflow) { /* overflow, so wrap around */
      dest_mask = mask[0];
      result.bv_array[dest_block] = result_chunk;
      dest_block++;
      result_chunk = ((bv_block_t) 0);
    }
  }

  if (dest_mask != mask[0]) {
    /* The last result_chunk was not copied back to result.bv_array
     in this case.  So let's do the needful.
    */
    result.bv_array[dest_block] = result_chunk;
  }

  cleanup(result);
  return result;
}


bool BVManager::checkBVGreaterThan(bvatom bv_inst1, bvatom bv_inst2)
{
  // Assume that bv_inst1 and bv_inst2 are already cleaned up

  if(bv_inst1.bv_width != bv_inst2.bv_width) {
    cerr<<"ERROR in BVManager::checkBVGreaterThan: Unequal widths of arguments of checkBVGreaterThan" << endl;
    assert(0);
  }
  else {
    for(unsigned long i = bv_inst1.bv_array_size-1; ; i--) {
      // An unsigned long is always >= 0, therefore it is meaningless to check
      // i >= 0 in the condition part of the for statement.
      
      if (bv_inst1.bv_array[i] > bv_inst2.bv_array[i]) {
	return true;
      }
      if (bv_inst1.bv_array[i] < bv_inst2.bv_array[i]) {
	return false;
      }
      
      if (i == 0) {
	break;
      }
    }
    return false;
  }
}

void BVManager::bv_exchange(bvatom& bv_inst1, bvatom& bv_inst2)
{
  std::swap(bv_inst1, bv_inst2);
}

bool BVManager::checkBVEquality(bvatom bv_inst1, bvatom bv_inst2)
{
  // Assume that bv_inst1 and bv_inst2 are already cleaned up

  if(bv_inst1.bv_width != bv_inst2.bv_width) {
    cerr<<"ERROR in BVManager::checkBVEquality: Unequal widths of arguments of checkBVEquality" << endl;
    assert(0);
  }
  else {
    for(unsigned long i = bv_inst1.bv_array_size-1; ; i--) {
      // An unsigned long is always >= 0, therefore it is meaningless to check
      // i >= 0 in the condition part of the for statement.
      
      if (bv_inst1.bv_array[i] != bv_inst2.bv_array[i]) {
	return false;
      }
      
      if (i == 0) {
	break;
      }
    }
    return true;
  }
}

bool BVManager::checkBVForZero(bvatom bv_inst1)
{
  // Assume that bv_inst1 is cleaned up

  for(unsigned long i = bv_inst1.bv_array_size - 1; ; i--) {
    // An unsigned long is always >= 0, therefore it is meaningless to check
    // i >= 0 in the condition part of the for statement.
    
    if (bv_inst1.bv_array[i] != ((bv_block_t) 0)) {
      return false;
    }
    
    if (i == 0) {
      break;
    }
  }
  
  return true;
}

void BVManager::bv_copy(bvatom& bv_src, bvatom& bv_dest)
{
  if(bv_src.bv_array == bv_dest.bv_array) {
    return;
  }
  else {
    deleteBV(bv_dest);
    bv_dest = getZeroedBVatom(bv_src.bv_width);
    // bv_dest is a cleaned up bvatom
    replaceBySelBVatom(bv_src, bv_dest, 0, 0, bv_dest.bv_width);
    // replaceBySelBVatom ensures bv_dest stays cleaned up
  }
}

void BVManager::deleteBV(bvatom& bv_inst)
{
    //COMMENTED: To avoid memory corruption caused since bv_array
    //might not have been initialized (no constructor exists)

  if (bv_inst.bv_array != NULL) {
    delete[] bv_inst.bv_array;
    bv_inst.bv_array = NULL;
  }
  bv_inst.bv_array=NULL;
  bv_inst.bv_array_size=0;
  bv_inst.bv_width=0;
}
