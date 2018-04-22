#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include "revsyn_ttb.hpp"
#include <time.h>
int factor( int fac ){
	if( fac == 0 ) return 1;
	int ret = 1;
	for( int i=1; i<=fac; i++ )
		ret *= i;
	return ret;
}

class SpecData{
public:
	size_t diff[2],sz;
	std::string fprefix;
	SpecData(){
		diff[0]=0;
		diff[1]=0;
		sz=0;
	}
};

void vecinc( std::vector<int>& vec, int id ){
	while( id>=vec.size() )
		vec.push_back(0);
	vec[id]++;
}

int main( int argc, char * argv[] ){
	/**
	int mode = 0;
	if( argc<2 )
		return 0;
	if( argc>2 )
		mode = atoi(argv[2]);
	Top_Ttb_t * pTtb = Ttb_ReadSpec( argv[1] );
	tRevNtk * pRev;
	if( mode == 1 )
		pRev = Top_TtbToRev_Bi(pTtb);
	else if( mode == 3 )
		pRev = Top_TtbToRev_Bi_Core(pTtb);
	else 
		pRev = Top_GBDL(pTtb);/**/
	char * path = NULL;
	if( argc>1 )
		path = argv[1];
	int dim  = 2;
	int size = 1<<dim;
	int permu= factor(size);
	int comb = 1<<size; //care condition combinations
	int M[2] = {0,0};
	std::string M0, M1, M2;
	SpecData sd[3];
	size_t incmp_sz=0;
	std::vector<int> BDsz;
	std::vector<int> GBDsz;
	std::vector<int> GBDLsz;
	std::vector<int> n0BDsz;
	std::vector<int> n0GBDsz;
	std::vector<int> n0GBDLsz;
	std::ofstream om1("m1_static.log",std::ios::out), om2("m2_static.log",std::ios::out), om3("m3_static.log",std::ios::out);
	printf("dim=%8d,size=%8d,permu=%8d,com=%8d\n",dim,size,permu,comb);
	printf("round=0");
	for( int i=1; i<=permu; i++){
		for( int j=0; j<comb; j++ ){
			tRevNtk * pRev[3];
			Top_Ttb_t * pTtb[2];
			char fprefix[256],fcmp[256],fincmp[256];
			sprintf(fprefix,"%d_%d",i,j);
			sprintf(fcmp,"%s%s%d_%d.cmp",path?path:"",path?"/":"",i,j);
			sprintf(fincmp,"%s%s%d_%d.incmp",path?path:"",path?"/":"",i,j);
			//printf("%s\n",fcmp);
			//printf("%s\n",fincmp);
			pTtb[0] = Ttb_ReadSpec(fcmp);
			pTtb[1] = Ttb_ReadSpec(fincmp);
			pRev[0] = Top_TtbToRev_Bi(pTtb[0]);
			pRev[1] = Top_TtbToRev_Bi_Core(pTtb[1]);
			pRev[2] = Top_GBDL(pTtb[1]);

			
			// Max: M1(BD>GBD) M2(GBD>GBDL)
			// Priority: M1 > M2 > (M1 & M2)
			// If (M1 & M2) then maximize M1, maximize M2;
			int LocalM[2] = {0,0};
			int prevM[2]={M[0],M[1]};
			LocalM[0] = pRev[0]->size() - pRev[1]->size();
			LocalM[1] = pRev[1]->size() - pRev[2]->size();
			if( LocalM[0]>M[0] ){
				M0 = fprefix;
				M[0] = LocalM[0];
				om1 << M0 <<std::endl;
			}
			if( LocalM[1]>=M[1] && !pTtb[1]->empty() ){
				M1 = fprefix;
				M[1] = LocalM[1];
				om2 << M1 <<std::endl;
			}
			if( !pTtb[1]->empty() && LocalM[0]>0 && LocalM[1]>=0 ){
				M2 = fprefix;
				incmp_sz = pTtb[1]->size();
				om3 << M2 <<std::endl;
			}

			printf("\rround=%d",i);
			vecinc( BDsz, pRev[0]->size() );
			vecinc( GBDsz, pRev[1]->size() );
			vecinc( GBDLsz, pRev[2]->size() );
			if( !pTtb[1]->empty() ){
				vecinc( n0BDsz, pRev[0]->size() );
				vecinc( n0GBDsz, pRev[1]->size() );
				vecinc( n0GBDLsz, pRev[2]->size() );
			}
			delete pTtb[0];
			delete pTtb[1];
			delete pRev[0];
			delete pRev[1];
			delete pRev[2];
		}
	}
	om1.close();
	om2.close();
	om3.close();
	printf("\n");
	//delete pTtb;
	//delete pRev;
	if( !M0.empty() )
		std::cout<<"GBD better than BD in "<< M0<<std::endl;
	if( !M1.empty() )
		std::cout<<"GBDL better than GBD in "<< M1<<std::endl;
	
	if( !M0.empty() && !M1.empty())
		std::cout<<"GBDL better than GBD better than BD in "<< M2<<std::endl;

	long long accum = 0, incmp_accum = 0;

	accum = 0;
	for( int i=0; i<BDsz.size(); i++ ){
		printf("%10d", BDsz[i] );
		accum += BDsz[i];
	}
	std::cout <<" ="<<accum<< std::endl;

	accum = 0;
	for( int i=0; i<GBDsz.size(); i++ ){
		printf("%10d", GBDsz[i] );
		accum += GBDsz[i];
	}
	std::cout <<" ="<<accum<< std::endl;

	accum = 0;
	for( int i=0; i<GBDLsz.size(); i++ ){
		printf("%10d", GBDLsz[i] );
		accum += GBDLsz[i];
	}
	std::cout <<" ="<<accum<< std::endl;

	incmp_accum = 0;
	for( int i=0; i<n0BDsz.size(); i++ ){
		printf("%10d", n0BDsz[i] );
		incmp_accum += n0BDsz[i];
	}
	std::cout <<" ="<<incmp_accum<< std::endl;

	incmp_accum = 0;
	for( int i=0; i<n0GBDsz.size(); i++ ){
		printf("%10d", n0GBDsz[i] );
		incmp_accum += n0GBDsz[i];
	}
	std::cout <<" ="<<incmp_accum<< std::endl;
	incmp_accum = 0;
	for( int i=0; i<n0GBDLsz.size(); i++ ){
		printf("%10d", n0GBDLsz[i] );
		incmp_accum += n0GBDLsz[i];
	}
	std::cout <<" ="<<incmp_accum<< std::endl;
	printf("accum of cmp = %10lld, accum of incmp = %10lld\n", accum, incmp_accum );
}
/**/

