BIN = bin
MAPS = MaroonV

# Compila SpeedUp
$(BIN)/SpeedUp: $(MAPS)/SpeedUp.cc
	g++ -g $(MAPS)/SpeedUp.cc -pthread -o $@

clean:
	@rm -f *.o* $(BIN)/*
