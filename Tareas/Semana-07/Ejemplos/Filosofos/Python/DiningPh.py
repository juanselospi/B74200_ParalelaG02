import threading
import sys
import time

"""
    States represent the philosopher state in any moment
"""
class States:
    Thinking, Hungry, Eating = range( 0, 3 )

"""
    Represents the Silberschatz solution using a monitor
    We use a lock to allow only one thead running in all 'public' (PickUp, and PutDown) methods of this class
    Each chopstick is represented by a Condition variable
"""
class DinningPhi:
# Instance variables
    n = 0
    lock = 0
    state = []
    chopstick = []

# Class variable initialization
    def __init__( self, number ):
        self.lock = threading.Lock()
        self.n = number
        self.chopstick = [threading.Condition( self.lock ) for i in range( number ) ]
        self.state = [ States.Thinking for i in range( number ) ]

# Called when a philosopher wants to eat, and needs to pickup two chopsticks
    def PickUp( self, who ):
        with self.lock:
            self.state[ who ] = States.Hungry
            self.Test( who )
            if ( self.state[ who ] == States.Hungry ):
                self.chopstick[ who ].wait( )

# Called when a philosopher finish eating and needs to return his two chopsticks
    def PutDown( self, who ):
        with self.lock:
            self.state[ who ] = States.Thinking
            self.Test( ( who + (self.n - 1) ) % self.n )
            self.Test( ( who + 1 ) % self.n )

# Check if a philosopher can pickup his two chopsticks, if none of his neighbor are not eating
# Additionally check when a philosopher are returning his chopstick if one of his neighbor are Hungry
    def Test( self, who ):
        if ( ( self.state[ (who + (self.n - 1) ) % self.n ] != States.Eating ) and
             ( self.state[ (who + 1) % self.n ] != States.Eating ) and
             ( self.state[ who ] == States.Hungry ) ):
            self.state[ who ] = States.Eating
            self.chopstick[ who ].notify( )

# Print table status
    def Print( self, who ):
        desc = [ 'Thinking', 'Hungry', 'Eating' ]
        with self.lock:
            sys.stdout.write( "Filo %s [ " % who )
            [ sys.stdout.write( " %s, " % desc[ self.state[ i ] ] ) for i in range( self.n ) ]
            sys.stdout.write( "]\n" )

# Code for the thread representing each philosopher
def Filosofo( who, mesa ):

    for i in range(10):
        time.sleep(0.1)                 # think
        sys.stdout.write( "    Filosofo %s va a tomar palillos \n" % who )
        mesa.PickUp( who )
        sys.stdout.write( "    Filosofo %s esta comiendo \n" % who )
        time.sleep(0.1)                 # (yield makes deadlock more likely)
        mesa.PutDown( who )
        sys.stdout.write( "    Filosofo %s va a pensar \n" % who )
        time.sleep(0.1 * who )
        mesa.Print( who )

# Code to start simulation, original problem states five philosophers
def Festin( number ):
    mesa = DinningPhi( number )
    for i in range( number ):
        t = threading.Thread( target=Filosofo, args=(i, mesa) )
        t.start()

def main():
    Festin( 5 )

if __name__ == "__main__":
    main()

