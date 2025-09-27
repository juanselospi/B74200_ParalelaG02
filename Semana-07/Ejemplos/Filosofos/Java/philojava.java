/**
 *  This class presents a solution to Dijkstra problem "Dining Philosophers"
 *  The solution is based on material presented on Silberschatz's book of Operating Systems
 *  diningph, represents the "monitor", this code is how to is that monitor
 *
 */

import java.util.Random;
import java.util.concurrent.Semaphore;


public class philojava extends Thread {

   private static final int NUMTHRDS = 5;
   private int who;
   private Semaphore mutex;
   private diningph dp;

   public philojava( int i, diningph dp ) {
      this.who = i;
      this.mutex = new Semaphore( 1 );
      this.dp = dp;

   }


/**
 * Each thread will run this method, representing a philosopher
 * On every turn the philospher will pickup chopsticks, eat, return chopsticks, and think
 */
   public void run(){

      int eats, thinks;
      Random r = new Random();

      for ( int i = 0; i < 10; i++ ) {

         print( "Philo # " + who + ", round " + i + "\n" );

         dp.pickup( who);
         eats = r.nextInt( 5 ) * 1000;
         try {
            sleep( eats );
         } catch ( InterruptedException e ) {
            System.out.println( e );
         }

         dp.putdown( who);
         thinks = r.nextInt( 5 ) * 1000;
         try {
            sleep( thinks );
         } catch ( InterruptedException e ) {
            System.out.println( e );
         }
      }

      print( "Thread finishing ... " + who );

   }

/**
 * 
 */
   private void print( String s ) {
      try {
         mutex.acquire();
      } catch  ( InterruptedException e ) {
         System.out.println( e );
      }
      System.out.println( s );
      mutex.release();

   }

/**
 * Starts the philosophers
 */
   public static void main ( String[] args ) {
      diningph dp;

      dp = new diningph();		// Create only one instance, passed by parameter to each thread

      for ( int i = 0; i < NUMTHRDS; i++ ) {
          philojava p = new philojava( i, dp );
          p.start();
      }

   }

}

