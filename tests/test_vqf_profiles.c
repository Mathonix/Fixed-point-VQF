#include "fixed_vqf.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
int main(void){
 fixed_vqf_t s; q24_t g[3]={-225473,70276,125912},bias[3]; q30_t acc[3]={0,0,1073741824};
 assert(sizeof(s)==496);fixed_vqf_init(&s);assert(s.profile==1);
 const uint16_t tau[4]={1000,2500,5000,2500};
 for(unsigned p=0;p<4;p++){
  assert(fixed_vqf_tau_acc_ms(p)==tau[p]);const q30_t *c=fixed_vqf_gyro_coeffs(p);
  assert((int64_t)c[0]+c[1]+c[2]==1073741824LL+c[3]+c[4]);
  assert(c[4]>-1073741824 && c[4]<1073741824);assert(1073741824LL+c[3]+c[4]>0);assert(1073741824LL-c[3]+c[4]>0);
  fixed_vqf_init(&s);fixed_vqf_set_profile(&s,p);assert(s.profile==(p<3?p:1));
  /* Allow the slowest rest bias tune to fully converge before checking accuracy. */
  for(unsigned i=0;i<90*FIXED_VQF_SAMPLE_HZ;i++){fixed_vqf_update_gyr(&s,g);fixed_vqf_update_acc(&s,acc);}
  assert(fixed_vqf_get_rest_detected(&s));fixed_vqf_get_bias_q24(&s,bias);
  for(unsigned k=0;k<3;k++){ printf("profile %u axis %u residual Q24 %ld\n",p,k,(long)(bias[k]-g[k]));fflush(stdout);assert(llabs((long long)bias[k]-g[k])<600); }
  for(unsigned next=0;next<4;next++){
   q30_t gq[4],aq[4];int64_t b[3],cov[9];memcpy(gq,s.gyr_q,sizeof(gq));memcpy(aq,s.acc_q,sizeof(aq));memcpy(b,s.gyro_bias_q32,sizeof(b));memcpy(cov,s.bias_P_q20,sizeof(cov));
   fixed_vqf_set_profile(&s,next);
   assert(!memcmp(gq,s.gyr_q,sizeof(gq)) && !memcmp(aq,s.acc_q,sizeof(aq)) && !memcmp(b,s.gyro_bias_q32,sizeof(b)) && !memcmp(cov,s.bias_P_q20,sizeof(cov)));
   assert(s.profile==(next<3?next:1));
   for(unsigned i=0;i<10;i++){fixed_vqf_update_gyr(&s,g);fixed_vqf_update_acc(&s,acc);}
  }
 }
 fixed_vqf_set_profile(&s,255);assert(s.profile==1);
 puts("VQF profiles: coefficients/DC/poles/static-bias/state-preservation passed");
 return 0;
}
