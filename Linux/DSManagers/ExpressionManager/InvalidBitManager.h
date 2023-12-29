#include "HashTable_WithStandardKeys.h"
#include "LogManager.h"
#include "ExpressionValue.h"

class t_Expression;
class t_InvalidBitManager
{
 private:
  t_HashTable<string, t_Expression*> m_hashTableOfExpressionToInvalidBitExpression;
  t_Logger* m_logManger;
  ostream* m_log_file;
 public:
  bool setInvalidBit(string& key, t_Expression* invalid_bit_expr);
  t_Expression* getInvalidBitExpression(string& key);
  bool isInvalidBitPresent(string& key);
  bool removeInvalidBitExpression(string& key);

  t_Expression* getInvalidBitOfIthAtomInConcat(t_Expression* concat_expression, unsigned int atom_position_from_lsb, t_ExpressionImplementationManager*& expressionImplementationManagerInstance, int expressionImplementationType);
};


