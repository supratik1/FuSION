#include "ExpressionManager.h"

namespace operations
{
    struct ExpressionPtr
    {
	t_Expression* ptr;
	explicit ExpressionPtr(t_Expression* e):ptr(e){}
	operator t_Expression*(){return ptr;}
    };

#define CREATE_OPERATOR(Oper,OperName)                                                           \
    inline ExpressionPtr operator Oper(ExpressionPtr a, ExpressionPtr b)                         \
    {                                                                                            \
	t_ExpressionManager *em=t_ExpressionManager::getSingleton();                             \
	assert(a.ptr!=NULL);                                                                     \
	assert(b.ptr!=NULL);                                                                     \
	assert(em!=NULL);                                                                        \
	assert(em->getWidth(a) == 1);                                                            \
	assert(em->getWidth(b) == 1);                                                            \
	t_Expression* e = em->createOneBitExpressionWithTwoOperands(#OperName, a.ptr, b.ptr);    \
	assert(e!=NULL);                                                                         \
	return ExpressionPtr(e);                                                                 \
    }                                                                                            \
    inline ExpressionPtr operator Oper(ExpressionPtr a, t_Expression* b)                         \
    {    return a Oper ExpressionPtr(b); }
    //-------------------- DONE --------------------


    CREATE_OPERATOR(&&,logand)
    CREATE_OPERATOR(||,logor)
    CREATE_OPERATOR(^, logxor)

    inline ExpressionPtr ite(ExpressionPtr a,ExpressionPtr b,ExpressionPtr c)
    {
	t_ExpressionManager *em=t_ExpressionManager::getSingleton();                             
	assert(a.ptr!=NULL);                                                                     
	assert(b.ptr!=NULL);                                                                     
	assert(c.ptr!=NULL);
	assert(em!=NULL);                                                                        
	assert(em->getWidth(a) == 1);                                                            
	assert(em->getWidth(b) == 1);                                                            
	assert(em->getWidth(c) == 1);
	t_Expression* e = em->createOneBitExpressionWithThreeOperands(#OperName, a.ptr, b.ptr,c.ptr);    
	assert(e!=NULL);                                                                         
	return ExpressionPtr(e);                                                                 
    }

    inline ExpressionPtr ite(ExpressionPtr a,t_Expression* b,t_Expression* c)
    {
	return ite(a,ExpressionPtr(b),ExpressionPtr(c));
    }
}
