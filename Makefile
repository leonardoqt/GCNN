ROOT_DIR=$(shell pwd)
ODIR  = $(ROOT_DIR)/obj
SDIR  = $(ROOT_DIR)/src

CXX   = g++
CFLAG = -std=c++11
 
SRC   = $(shell ls $(SDIR)/*.cpp)
OBJ   = $(patsubst $(SDIR)/%.cpp,$(ODIR)/%.o,$(SRC))

gcnn.x : $(OBJ)
	$(CXX) -o $@ $^ $(CFLAG)

$(ODIR)/main.o : $(SDIR)/main.cpp | $(ODIR)/.
	$(CXX) -c -o $@ $< $(CFLAG)

$(ODIR)/%.o : $(SDIR)/%.cpp $(SDIR)/%.h | $(ODIR)/.
	$(CXX) -c -o $@ $< $(CFLAG)

%/. : 
	mkdir -p $(patsubst %/.,%,$@)
	
.PRECIOUS: %/.
.PHONY: clean clean_test clean_all
clean:
	rm -rf *.x $(ODIR)
clean_test:
	rm -rf *.dat
clean_all:
	rm -rf *.x $(ODIR) *.dat
