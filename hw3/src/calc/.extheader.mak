calc.d: ../../include/calcGNum.h 
../../include/calcGNum.h: calcGNum.h
	@rm -f ../../include/calcGNum.h
	@ln -fs ../src/calc/calcGNum.h ../../include/calcGNum.h
