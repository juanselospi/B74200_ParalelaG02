
public class ConditionJava {

   public synchronized void Wait( ) {
      try {
         this.wait();
      }
      catch ( InterruptedException e ) {
         System.out.println( e );
         System.exit( 2 );
      }
   }

   public synchronized void Signal( ) {
      this.notify();
   }

}

