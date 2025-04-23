float round(float arg);
static void step();

int *time;
int *hb;
int pointer;
float lag_60, lag_15, lag_1;

void create()
{
  int i;
  if(clonep(this_object())) destruct(this_object());
  call_out("step",10);
  time = allocate(360);
  hb = allocate(360);
  for(i=1;i<360;i++)
    time[i]=hb[i]=-1;
  time[0]=time();
  hb[0]=absolute_hb_count();
  pointer=0;
  lag_60=lag_15=lag_1=-1.0;
}

static void step()
{
  int pointer_60, pointer_15, pointer_1;
  
  call_out("step",10);
  time[pointer]=time();
  hb[pointer]=absolute_hb_count();
  pointer_60=(pointer==359?0:pointer+1);
  pointer_15=(pointer<90?pointer-90+359:pointer-90);
  pointer_1=(pointer<6?pointer-6+359:pointer-6);
  
  if(time[pointer_1]>=0)
  {
    lag_1 = ((float)hb[pointer]-(float)hb[pointer_1])*2.0;
    lag_1 /= (float)(time[pointer]-time[pointer_1]);
    lag_1 = 100.0*(1.0-lag_1);
  }
  if(time[pointer_15]>=0)
  {
    lag_15 = ((float)hb[pointer]-(float)hb[pointer_15])*2.0;
    lag_15 /= (float)(time[pointer]-time[pointer_15]);
    lag_15 = 100.0*(1.0-lag_15);
  }
  if(time[pointer_60]>=0)
  {
    lag_60 = ((float)hb[pointer]-(float)hb[pointer_60])*2.0;
    lag_60 /= (float)(time[pointer]-time[pointer_60]);
    lag_60 = 100.0*(1.0-lag_60);
  }
  if(pointer==359)
    pointer=0;
  else
    pointer++;
}

float *read_lag_data()
{
  return ({lag_60,lag_15,lag_1});
}
