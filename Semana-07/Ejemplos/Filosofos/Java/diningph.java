/**
 *  Monitor emulation in Java using a lock to allow only one active method
 *  Use condition variables to control concurrency
 */

import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.ReentrantLock;

/**
 *  Monitor emulation is done using this Java class
 *  From this class only "pickup" and "putdown" are used externally and
 *  these are controlled with lock access (only one active in monitor)
 */
public class diningph {

   private static final int NUMTHRDS = 5;

   private enum State { Thinking, Hungry, Eating }
   private State[] state;		// Each of the philosophers state
   private Lock lock;			// To isolate each monitor method
   private Condition self [];		// To control access to each chopstick

/**
 *  Class constructor
 */
   public diningph() {

      this.state = new State[ NUMTHRDS ];
      this.lock = new ReentrantLock();
      this.self = new Condition[ NUMTHRDS ];
      for ( int chopstick = 0; chopstick < NUMTHRDS; chopstick++ ) {
         this.state[ chopstick ] = State.Thinking;
         this.self[ chopstick ] = lock.newCondition();
      }

   }


/**
 *  Each philosopher must call this method to check access to his chopsticks
 *  If available, he can eat, otherwise must wait
 */
   public void pickup( int who ) {
    int number;

    this.lock.lock();

    this.state[ who ] = State.Hungry;
    number = who + 1;
    System.out.println( "Philosopher " + who + ", will try to pickup sticks" );

    print( number );
    test( who );
    if ( this.state[ who ] == State.Hungry ) {
        System.out.println( "Philosopher " + who + " sticks are busy, will wait for them " );
        try {
           this.self[ who ].await();
        }
        catch ( InterruptedException e ) {
           System.out.println( e );
        }
    }

    System.out.println( "Philosopher " + who + " will start eating" );

    this.lock.unlock();

   }


/**
 *  Once the philospher finish eating, he must return the chopsticks to the table
 *  This method checks if one of his neighbors are hungry to let them eat
 */
   public void putdown( int who ) {
      int number;

      this.lock.lock();

      number = who + 1;
      System.out.println( "Philosopher " + who + " end eating, will putdown sticks" );
      this.state[ who ] = State.Thinking;
      test( (who + 4) % NUMTHRDS );
      test( (who + 1) % NUMTHRDS );

      this.lock.unlock();

   }


/**
 *  Check if philosopher can have his two chopsticks and eat
 *  If not, his state is not changed
 */
   private void test( int who ) {
      int number;

      if ( ( this.state[ (who + 4) % NUMTHRDS ] != State.Eating ) && 
           ( this.state[ who ] == State.Hungry ) && 
           ( this.state[ (who + 1) % NUMTHRDS ] != State.Eating ) ) {
           this.state[ who ] = State.Eating;
           this.self[ who ].signal();
       }

   }


/**
 *  Print a simulation status
 */
   public void print( int who ) {

      for ( int i = 0; i < NUMTHRDS; i++  ) {
          System.out.printf( "(%d) Philosopher %d is %s \n", who, i + 1, (state[i]==State.Hungry)?"Hungry":(state[i]==State.Thinking)?"Thinking":"Eating");

      }

   }

}

