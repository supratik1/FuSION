#include<vector>
/*
vector<int> t_Expression::getIndicesForRead( t_Expression* arrayExpr, int lb, int ub)
{
    assert(arrayExpr!=NULL);
    assert(lb<=ub);
    assert(lb>=0);

    pair< vector<t_MemoryBounds>, int> res = t_ExpressionManager::getSingleton()->getBoundsOfArray(arrayExpr);
    assert(res.second>=0);
    assert(res.second< res.first.size());

    vector<t_MemoryBounds> boundsArrayExpr( res.first.begin()+res.second, res.first.end());

    if(boundsArrayExpr.empty())
	return boundsArrayExpr;

    vector<unsigned> dimWidths(boundsArrayExpr.size());
    {
	unsigned lastDimWidth = 1;
	for(int i=boundsArrayExpr.size()-1; i>=0; --i){
	    dimWidths.push_back(lastDimWidth);
	    unsigned wd = boundsArrayExpr[i].m_upperbound - boundsArrayExpr[i].m_lowerbound + 1;
	    lastDimWidth *= wd;
	}
	dimWidths.push_back(lastDimWidth);
	reverse(dimWidths.begin(), dimWidths.end());
    }

    unsigned width = ub-lb+1;
    vector<int> selResult;
    int rem = lb;
    while(dimWidths[ selResult.size() ] > width ){ //more finer select required
	//unsigned wd = boundsArrayExpr[ selResult.size() ].m_upperbound - boundsArrayExpr[ selResult.size() ].m_lowerbound + 1;
	unsigned index = rem / dimWidths[selResult.size()+1]; //index in the current dimension
	rem = rem % dimWidths[ selResult.size()+1];           //remaining size of the array i.e. read on [4:0][1:0] will result [1:0]
	selResult.push_back(index);
    }    

    return selResult;
}
*/
#include<iterator>
#include<algorithm>
#include<iostream>
int main()
{
    using namespace std;
    //mem[1:0][4:0][1:0]
    unsigned dimWidths[]={20,10,2,1};
    int lb=0,ub=1;
   unsigned width = ub-lb+1;
    vector<int> selResult;
    int rem = lb;
    while(dimWidths[ selResult.size() ] > width ){ //more finer select required
	//unsigned wd = boundsArrayExpr[ selResult.size() ].m_upperbound - boundsArrayExpr[ selResult.size() ].m_lowerbound + 1;
	unsigned index = rem / dimWidths[selResult.size()+1];
	rem = rem % dimWidths[ selResult.size()+1];
	selResult.push_back(index);
    }

    copy(selResult.begin(), selResult.end(), ostream_iterator<int>(cout,", "));
    cout<<endl;
    return 0;
}
