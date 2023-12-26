#include "InvalidBitManager.h"
#include "Expression.h"

bool t_InvalidBitManager::setInvalidBit(string& key, t_Expression* invalid_bit_expr)
{
  if(invalid_bit_expr==NULL)
    {
      return false;
    }
  t_HTStatusValue<t_Expression*> result_search = m_hashTableOfExpressionToInvalidBitExpression.hashtable_search(key);
  if(result_search.success()==true)
    {
      t_HTStatusValue<t_Expression*> delete_result = m_hashTableOfExpressionToInvalidBitExpression.hashtable_delete(key);
      if(delete_result.success()==false)
	{
	  // Error Unable to delete invalid bit expression from hashtable.
	  return false;
	}
    }
  t_HTStatusValue<t_Expression*> result_insert = m_hashTableOfExpressionToInvalidBitExpression.hashtable_insert(key, invalid_bit_expr);
  if(result_insert.success()==false)
    {
      // Error Unable to insert a invalud_bit_expr value in hashtable
      return false;
    }
  //  cout<<" Setting invalid bit of "<<expr->getID()<<" to "<<invalid_bit_expr->getID()<<endl;
  return true;
}

t_Expression* t_InvalidBitManager::getInvalidBitExpression(string& key)
{
  t_HTStatusValue<t_Expression*> result_get = m_hashTableOfExpressionToInvalidBitExpression.hashtable_search(key);
  if(result_get.success()==false)
    {
      // No invalid_bit_expresison found for the current expression expr.
      return NULL;
    }
  else
    {
      t_Expression* res = result_get.getValue();
      //  cout<<" Getting invalid bit of "<<expr->getID();
      //      cout<<" to "<<res->getID()<<endl;
      return res;
    }
}

bool t_InvalidBitManager::isInvalidBitPresent(string& key)
{
  t_HTStatusValue<t_Expression*> result_search = m_hashTableOfExpressionToInvalidBitExpression.hashtable_search(key);
  if(result_search.success()==false)
    {
      // No invalid_bit_expresison found for the current expression expr.
      return false;
    }
  else
    {
      return true;
    }
}

bool t_InvalidBitManager::removeInvalidBitExpression(string& key)
{
  t_HTStatusValue<t_Expression*> result_delete = m_hashTableOfExpressionToInvalidBitExpression.hashtable_delete(key);
  if(result_delete.success()==false)
    {
      // No invalid_bit_expresison found for the current expression expr.
      return false;
    }
  else
    {
      return true;
    }
}
/*
t_Expression* t_InvalidBitManager::getInvalidBitOfIthAtomInConcat(t_Expression* concat_expression, unsigned int atom_position_from_lsb, t_ExpressionImplementationManager*& expressionImplementationManagerInstance, int expressionImplementationType)
{
  if(concat_expression->getLabelOfExpression(expressionImplementationType, expressionImplementationManagerInstance)!="concat")
    {
      return getInvalidBitExpression(concat_expression);
    }
  vector<t_Expression*> operands_of_concat = concat_expression->getVectorOfOperands(expressionImplementationType, expressionImplementationManagerInstance);
  if(atom_position_from_lsb >= operands_of_concat.size() || atom_position_from_lsb < 0)
    {
      return NULL;
    }
  else
    {
      t_Expression* current_atom_expr = operands_of_concat[operands_of_concat.size()-atom_position_from_lsb-1];
      t_Expression* current_atom_invalid_bit = getInvalidBitExpression(current_atom_expr);
      return current_atom_invalid_bit;
    }
}

*/
