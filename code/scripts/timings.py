import sys
# Usage timing <infile> 
fn = sys.argv[1]
f=open(fn,"r")
while True:
  r1=f.readline()
  if len(r1) == 0: break
  r2=f.readline()
  r3=f.readline()
  t0=float(r1.split()[-1])
  t1=float(r2.split()[-1])
  r3=r3.strip('\r').strip('\n')
  r3 = r3 + str(t1-t0)
  print r3
f.close()