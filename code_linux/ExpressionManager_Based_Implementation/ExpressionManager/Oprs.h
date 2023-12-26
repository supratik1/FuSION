/********************************************************************
  This code was developed entirely by Ashutosh Kulkarni and Supratik
  Chakraborty at IIT Bombay, India.  No part of this code may be used
  for commercial or other purposes without explicit permission from
  supratik@cse.iitb.ac.in

  Last updated: May 25, 2013
*********************************************************************/

#ifndef oprh_h
#define oprh_h
#include<algorithm>
#include<string>
#include<list>
#include<fstream>

using namespace std;
#define BVID
#define NUM_INTS 8
#define BYTE_SIZE 8

typedef unsigned long bv_block_t;

typedef struct bv_internal_type {
  bv_block_t *bv_array;
  unsigned long bv_array_size;
  unsigned long bv_width;
  list<bv_block_t *>::iterator list_of_bv_blocks_iterator;
} bvatom;

class BVManager
{
 private:
  unsigned long width_of_max_representable_int;
  unsigned long block_size;
 public:
  unsigned long avail_block_size;
  bv_block_t *mask, *cumulative_mask;
  
 private:
  unsigned long cumulative_array_size;
  unsigned long hex_width, hex_blocks_in_block;
  bv_block_t  *hex_blocks_mask;
  std::list<bv_block_t *> list_of_bv_blocks;

  // Keeping track of all bvatoms allocated
  unsigned long get_bv_array_size(unsigned long w);
  
  void         freeBVatom(bvatom bv_inst) ;
  
  unsigned long get_len_ignoring_left_zero_padding(bvatom bv_inst);
  void         printHexBV(bv_block_t * bv_inst, unsigned long width);
  bvatom       leftshiftBVatom(bvatom bv_inst, unsigned long shift, int preserve_width);
  bvatom       rightshiftBVatom(bvatom bv_inst, unsigned long shift, int preserve_width);
  bvatom       arith_div_mod(bvatom dividend, bvatom divisor, int div_mod_flag);
  inline unsigned long offset_from_width(unsigned long w);
  inline unsigned long block_from_width(unsigned long w);
  inline unsigned long offset_from_pos(unsigned long p);
  inline unsigned long block_from_pos(unsigned long p);
  inline unsigned long get_msb_offset(bvatom bv_inst);
  inline unsigned long get_msb_block(bvatom bv_inst);
  inline void cleanup(bvatom bv_inst);
 public:
  static BVManager *bvmanager_instance;

  static BVManager *getInstance();
  BVManager();
  ~BVManager();
  bvatom       getBVatom(unsigned long width, string &bv_str);
  bvatom       getZeroedBVatom(unsigned long width);
  bvatom       bitwise_and(bvatom bv_inst1, bvatom bv_inst2);
  bvatom       bitwise_or(bvatom bv_inst1, bvatom bv_inst2);
  bvatom       bitwise_xor(bvatom bv_inst1, bvatom bv_inst2);
  bvatom       bitwise_xnor(bvatom bv_inst1, bvatom bv_inst2);
  void         replaceBySelBVatom(bvatom bv_src, bvatom &bv_dest, 
				  unsigned long src_start, unsigned long dest_start, 
				  unsigned long length);

  bvatom       concat(bvatom bv_inst1, bvatom bv_inst2);
  bvatom       select_with_int(bvatom bv_inst1, unsigned long hi, unsigned long lo);
  bvatom       select_with_bv(bvatom bv_inst1, bvatom bv_hi, bvatom bv_lo);
  bvatom       bitwise_negate(bvatom bv_inst);
  bvatom       reduction_and(bvatom bv_inst);
  bvatom       reduction_or(bvatom bv_inst);
  bvatom       reduction_xor(bvatom bv_inst);
  bvatom       reduction_xnor(bvatom bv_inst);
  bvatom       reduction_nor(bvatom bv_inst);
  bvatom       reduction_nand(bvatom bv_inst);
  bvatom       arith_sub(bvatom bv_inst1, bvatom bv_inst2);
  bvatom       arith_add(bvatom bv_inst1, bvatom bv_inst2);
  bvatom       arith_signed_add(bvatom bv_inst1, bvatom bv_inst2);
  bvatom       arith_mult(bvatom bv_inst1, bvatom bv_inst2);
  bvatom       arith_div(bvatom dividend, bvatom divisor);
  bvatom       arith_mod(bvatom dividend, bvatom divisor);
  bvatom       arith_neg(bvatom bv_inst);
  bvatom       left_shift(bvatom bv_inst, bvatom bv_shift);
  bvatom       ext_left_shift(bvatom bv_inst, bvatom bv_shift);
  bvatom       right_shift(bvatom bv_inst, bvatom bv_shift);
  bvatom       reverseBV(bvatom bv_inst);
  bool         checkBVGreaterThan(bvatom bv_inst1, bvatom bv_inst2);
  bool         checkBVEquality(bvatom bv_inst1, bvatom bv_inst2);
  bvatom       BVExtend(bvatom bv_inst, unsigned long width, int sign_extend);
  bool         checkBVForZero(bvatom bv_inst);
  inline bool  isMsb1(bvatom bv_inst);
  void         printBVatom(bvatom bv_inst, int pretty);
  void         printPrettyBVatom(bvatom bv_inst);
  string       printBVasBinString(bvatom bv_inst);
  unsigned long convertBVToUnsignedInt(bvatom bv);
  void         deleteBV(bvatom& bv_inst);
  void         bv_exchange(bvatom& bv_inst1, bvatom& bv_inst2);
  void         bv_copy(bvatom& bv_src,bvatom& bv_dest);

  #ifdef QE //#ifdef added by Ajith John on 9 June 2013
  /*New functions added by Ajith John */
  bvatom	addInverse(bvatom	bv_inst);
  bvatom	multInverse(bvatom	bv_inst);
  bool		findKappaAndOddPart(bvatom bv_inst, unsigned int &Kappa, bvatom &OddPart);
  bool		findKappa(bvatom bv_inst, unsigned int &Kappa); 
  bvatom	bvlshift(bvatom bv_inst, unsigned int shift);
  bvatom	bvrshift(bvatom bv_inst, unsigned int shift);
  bvatom	bvadd(bvatom bv_inst1, bvatom bv_inst2);
  bvatom	bvmul(bvatom bv_inst1, bvatom bv_inst2);
  bvatom	bvdiv(bvatom bv_inst1, bvatom bv_inst2);
  bvatom	bvmod(bvatom bv_inst1, bvatom bv_inst2);
  bvatom	bvsub(bvatom bv_inst1, bvatom bv_inst2);
  /*New functions added by Ajith John ends here*/
  #endif

};

inline unsigned long BVManager::offset_from_width(unsigned long w) 
{
  return ((w % avail_block_size == 0) ? (avail_block_size - 1) : ((w % avail_block_size) - 1));
}

inline unsigned long BVManager::block_from_width(unsigned long w) 
{
  return ((w/avail_block_size) - ((w % avail_block_size == 0) ? 1 : 0));
}

inline unsigned long BVManager::offset_from_pos(unsigned long p) 
{
  return (p % avail_block_size);
}

inline unsigned long BVManager::block_from_pos(unsigned long p) 
{
  return (p/avail_block_size);
}

inline unsigned long BVManager::get_msb_offset(bvatom bv_inst)
{
  return (offset_from_width(bv_inst.bv_width));
}

inline unsigned long BVManager::get_msb_block(bvatom bv_inst)
{
  return (bv_inst.bv_array_size - 1);
}

inline bool BVManager::isMsb1(bvatom bv_inst)
{
  return ((bv_inst.bv_array[get_msb_block(bv_inst)] & mask[get_msb_offset(bv_inst)]) != (bv_block_t) 0);
}

inline void BVManager::cleanup(bvatom bv_inst)
{
  bv_inst.bv_array[bv_inst.bv_array_size - 1] = 
    ((bv_inst.bv_array[bv_inst.bv_array_size - 1]) & 
     cumulative_mask[get_msb_offset(bv_inst)]);
}

#endif
