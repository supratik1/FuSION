#include "DagManager.h"
#include <algorithm>
#include "GC.h"
#include "helping_functions.h"
using namespace std;


int t_DAGManager :: collect_garbage()
{
  set<t_DAGNode *> global_set_of_garbage_nodes; //"Global" set of garbage nodes
  set<t_DAGNode *> global_set_of_nodes_with_edges_to_garbage; //"Global" set of non-garbage nodes



  LOG("--------------------------------------------------------------------\n");
  LOG("Starting Garbage collection \n");

  int garbage_nodes_counter = -1; //how many nodes collected as garbage
  
  bool init_gc_result = initializeGarbageCollector( global_set_of_garbage_nodes);
  if(!init_gc_result)
    {
      LOG("ERROR!! Failed initializing the garbage collector \n");
      LOG("Garbage Collection cannot proceed \n");
      resetGCDataStructures(global_set_of_garbage_nodes);
      return garbage_nodes_counter;
    }
  LOG("Initialized garbage collector successfully\n");

  set<t_DAGNode *> current_set_of_garbage_nodes;
  set<t_DAGNode *> children_of_current_set_of_garbage_nodes;
  set<t_DAGNode *> next_set_of_garbage_nodes;
  set<t_DAGNode *> maybe_non_garbage_nodes;

  bool init_current_result = initializeCurrentSetOfGarbageNodes(current_set_of_garbage_nodes, global_set_of_garbage_nodes);
  if(!init_current_result)
    {
      LOG("ERROR!! Failed initializing the global set of garbage nodes \n");
      resetGCDataStructures(global_set_of_garbage_nodes);
      return false;
    }
  LOG("Initialized current_set_of_garbage_nodes successfully\n");

  set<t_DAGNode *> :: iterator global_garbage_it;
  set<t_DAGNode *> :: iterator current_garbage_it;
  set<t_DAGNode *> :: iterator children_of_current_it;
  set<t_DAGNode *> :: iterator next_set_of_garbage_it;
  set<t_DAGNode *> :: iterator non_garbage_it;


  bool new_nodes_added = true;
  while(new_nodes_added)
    {
      //Step 2: Add all the children of all nodes in current_set_of_garbage_nodes 
      //to children_of_current_set_of_garbage_nodes
      LOG("Step 2: Adding children of current_set_of_garbage_nodes to children_of_current_set_of_garbage_nodes\n");
      for(current_garbage_it = current_set_of_garbage_nodes.begin(); current_garbage_it!= current_set_of_garbage_nodes.end(); current_garbage_it++)
	{
	  LOG("Processing node "+ (*current_garbage_it)->getLabel() + " \n");
	  t_DAGNode *current_node = (*current_garbage_it);
	  for(list<t_DAGNode *> :: iterator current_node_ol_it=current_node->m_out_list.begin(); current_node_ol_it!=current_node->m_out_list.end(); current_node_ol_it++)
	    {
	      children_of_current_set_of_garbage_nodes.insert(*current_node_ol_it);
	    }
     
	}
      LOG("Step 3: Adding nodes from children_of_current_set_of_garbage_nodes to next_set_of_garbage_nodes\n");
      for(children_of_current_it= children_of_current_set_of_garbage_nodes.begin(); children_of_current_it!= children_of_current_set_of_garbage_nodes.end(); children_of_current_it++)
	{
	  t_DAGNode *candidate_garbage_node = (*children_of_current_it);
	  if(checkIfDAGNodeIsGarbage(candidate_garbage_node, global_set_of_garbage_nodes))
	    {
	      LOG("Adding node "+candidate_garbage_node->getLabel()+"\n");
	      next_set_of_garbage_nodes.insert(candidate_garbage_node);
	    }
	  else
	    {
	      maybe_non_garbage_nodes.insert(candidate_garbage_node);
	    }
	}

      if(next_set_of_garbage_nodes.size() == 0)
	{
	  new_nodes_added = false;
	}

      for(next_set_of_garbage_it=next_set_of_garbage_nodes.begin(); next_set_of_garbage_it!= next_set_of_garbage_nodes.end(); next_set_of_garbage_it++)
	{
	  global_set_of_garbage_nodes.insert(*next_set_of_garbage_it);
	}
  
      current_set_of_garbage_nodes.clear();
      children_of_current_set_of_garbage_nodes.clear();


      for(next_set_of_garbage_it=next_set_of_garbage_nodes.begin(); next_set_of_garbage_it!=next_set_of_garbage_nodes.end();next_set_of_garbage_it++)
	{
	  current_set_of_garbage_nodes.insert(*next_set_of_garbage_it);
	}
      next_set_of_garbage_nodes.clear();

      
      for(non_garbage_it=maybe_non_garbage_nodes.begin(); non_garbage_it!=maybe_non_garbage_nodes.end();non_garbage_it++)
	{
	  global_set_of_nodes_with_edges_to_garbage.insert(*non_garbage_it);
	}
      maybe_non_garbage_nodes.clear();

    }


  LOG("Final global_set_of_garbage_nodes is \n");
  for(set<t_DAGNode *> :: iterator it=global_set_of_garbage_nodes.begin(); it!=global_set_of_garbage_nodes.end();it++)
    {

    }

 
  computeDifferenceOfGlobalSetOfGarbageNodesAndNonGarbageNodesAndDeleteInLists(global_set_of_garbage_nodes, global_set_of_nodes_with_edges_to_garbage);  
  deleteGarbageNodesAndFreeMemory( global_set_of_garbage_nodes);
  resetGCDataStructures( global_set_of_garbage_nodes);  
  LOG("--------------------------------------------------------------------\n");
  LOG("Finished Garbage collection \n");
  return garbage_nodes_counter;
}


bool t_DAGManager :: initializeGarbageCollector(set<t_DAGNode *>& global_set_of_garbage_nodes)
{
  LOG("Initializing the garbage collector\n");
  bool init_global_result = initializeGlobalSetOfGarbageNodes(global_set_of_garbage_nodes);
  if(!init_global_result)
    {
      LOG("ERROR!! Failed initializing the global set of garbage nodes \n");
      resetGCDataStructures( global_set_of_garbage_nodes);
      return false;
    }

  LOG("Finished initializing the garbage collector\n");
  return true;
}




bool t_DAGManager :: initializeGlobalSetOfGarbageNodes(set<t_DAGNode *> &global_set_of_garbage_nodes)
{
  t_DAGNode *current_node = this->m_head_nodes.m_next_node;

  int counter =0;
  if(current_node == NULL)
    {
      LOG("WARNING!!Garbage collector invoked when DAGManager has 0 DAGNodes in it\n");
      return false;
    }
  while(current_node != NULL)
    {
      if((current_node->m_reference_count == 0) && !(current_node->m_isRoot))
	//if((current_node->reference_count == 0))
	{
	  LOG(current_node->getLabel()+ " " + integerToString(current_node->m_reference_count)+"\n");
	  global_set_of_garbage_nodes.insert(current_node);
	  counter++;
	}
      else
	{
	  //Do nothing!!
	}
      current_node = current_node->m_next_node;
    }

  LOG("Initial set of garbage nodes contains "+integerToString(counter)+ " number of nodes \n");
  return true;
}


bool t_DAGManager :: initializeCurrentSetOfGarbageNodes(set<t_DAGNode *> &current_set_of_garbage_nodes, set<t_DAGNode *> &global_set_of_garbage_nodes)
{
  set<t_DAGNode *> ::iterator it;
  for(it=global_set_of_garbage_nodes.begin(); it!=global_set_of_garbage_nodes.end();it++)
    {
      current_set_of_garbage_nodes.insert((*it));
    }
  return true;
}


bool t_DAGManager :: computeDifferenceOfGlobalSetOfGarbageNodesAndNonGarbageNodesAndDeleteInLists(set<t_DAGNode *> &global_set_of_garbage_nodes, set<t_DAGNode *> global_set_of_nodes_with_edges_to_garbage)
{

  set<t_DAGNode *> nodes_to_remove_incident_edges_on;
  std::set_difference(global_set_of_nodes_with_edges_to_garbage.begin(), global_set_of_nodes_with_edges_to_garbage.end(),
		      global_set_of_garbage_nodes.begin(), global_set_of_garbage_nodes.end(),
		      std::inserter(nodes_to_remove_incident_edges_on, nodes_to_remove_incident_edges_on.end()));
  for(set<t_DAGNode *> ::iterator it = nodes_to_remove_incident_edges_on.begin();
      it!=nodes_to_remove_incident_edges_on.end(); it++)
    {
      LOG("Removing incident edges on "+(*it)->getLabel()+"\n");
      bool remove_inc_edges_result = removeEdgesFromNonGarbageNodesThatPointToGarbage(*it,global_set_of_garbage_nodes);
      if(!remove_inc_edges_result)
	{
	  LOG("ERROR!! Failed removing incident edges on a non-garbage node that point to garbage\n");
	  return false;
	}
    }
}

bool t_DAGManager :: removeEdgesFromNonGarbageNodesThatPointToGarbage(t_DAGNode *node, set<t_DAGNode *> &global_set_of_garbage_nodes)
{
  if(node->m_reference_count == 0)
    {
      LOG("ERROR!! Removing incident edges on a non-garbage node \n");
      
    }
  else
    {
      int inlist_size = node->m_in_list.size();
      int counter = 0;
      for(list<t_DAGNode *>::iterator inlist_it= node->m_in_list.begin();inlist_it!=node->m_in_list.end();inlist_it++)
	{
	  
	  //	  counter++;
	  t_DAGNode *parent = (*inlist_it);
	  set<t_DAGNode *>::iterator global_garbage_it;
	  global_garbage_it = global_set_of_garbage_nodes.begin();
	  global_garbage_it = global_set_of_garbage_nodes.find(parent);
	  if(global_garbage_it == global_set_of_garbage_nodes.end())
	    {
	      continue;
	    }
	  else
	    {
	      counter++;
	    }
	}
      LOG("Number of elements to erase is "+integerToString(counter) +"\n");
      int del_cntr = 0;
      while(del_cntr!=counter)
	{
	  for(list<t_DAGNode *>::iterator delit= node->m_in_list.begin(); delit!=node->m_in_list.end();delit++)
	    {
	      t_DAGNode *parent = (*delit);
	      set<t_DAGNode *>::iterator global_garbage_it;
	      global_garbage_it = global_set_of_garbage_nodes.begin();
	      global_garbage_it = global_set_of_garbage_nodes.find(parent);
	      if(global_garbage_it == global_set_of_garbage_nodes.end())
		{
		  continue;
		}
	      else
		{
		  del_cntr++;
	
		  node->m_in_list.erase(delit);
		  break;
		}
   
	    }
	}

    }
    return true;
}


bool t_DAGManager :: resetGCDataStructures(set<t_DAGNode *> &global_set_of_garbage_nodes)
{
  global_set_of_garbage_nodes.clear();
  return true;
}


bool t_DAGManager :: checkIfDAGNodeIsGarbage(t_DAGNode *candidate_garbage_node, set<t_DAGNode *> &global_set_of_garbage_nodes)
{
  set<t_DAGNode *>::iterator global_garbage_it;

  //Traverse the in-list of candidate_garbage_node and 
  // find if each node in it is in the global_set_of_garbage_nodes

  LOG("Testing node "+candidate_garbage_node->getLabel());
  for(list<t_DAGNode *>::iterator inlist_it = candidate_garbage_node->m_in_list.begin();
      inlist_it!=candidate_garbage_node->m_in_list.end(); inlist_it++)
    {
      global_garbage_it = global_set_of_garbage_nodes.find((*inlist_it));
      
      if(global_garbage_it == global_set_of_garbage_nodes.end())
	{
	  LOG("--->Is NOT Garbage \n");
	  return false;
	}
    }
  LOG("--->Is Garbage \n");
  return true;
}


bool t_DAGManager::deleteGarbageNodesAndFreeMemory(set<t_DAGNode *> &global_set_of_garbage_nodes)
{
  LOG("Deleting set of nodes from DAGManager\n");
  for(set<t_DAGNode *> :: iterator global_garbage_it= global_set_of_garbage_nodes.begin();
      global_garbage_it!=global_set_of_garbage_nodes.end(); global_garbage_it++)
    {
      t_DAGNode *node_to_delete = *global_garbage_it;
      LOG(node_to_delete->getLabel()+"\n");
      deleteANodeFromHashTable(node_to_delete);
      this->m_memory_usage = m_memory_usage - node_to_delete->m_signature.size() - node_to_delete->m_label.size() - sizeof(t_DAGNode);
      
      deleteNodeFromNodesList(node_to_delete);
      
      this->m_dagnode_count--;
      delete(node_to_delete);
    }
}


void t_DAGManager :: printASetOfDAGNodes(set<t_DAGNode *> set_to_print)
{
  LOG("printing a set\n");
  for(set<t_DAGNode *> ::iterator it =set_to_print.begin();it!=set_to_print.end();it++)
    {
      LOG(integerToString((long int)(*it)) + "   " + (*it)->getLabel() + "\n");
    }
  LOG("finished printing a set\n");
}
