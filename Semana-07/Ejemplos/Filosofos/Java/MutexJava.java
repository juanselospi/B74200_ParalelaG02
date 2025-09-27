
public class MutexJava {

   private boolean busy = false;

   public synchronized void Wait() {
      while ( busy ) {
         try {
             wait();
         } catch ( InterruptedException e ) {
           System.out.println( e );
           System.exit( 2 );
         }
      }
      busy = true;
   }

   public synchronized void Signal() {
      busy = false;
      notify();
   }

}

