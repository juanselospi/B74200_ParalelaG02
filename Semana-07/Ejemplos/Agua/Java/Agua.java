import java.util.concurrent.Semaphore;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.Random;

//A shared resource/class
class Shared {
    AtomicInteger cH;
    AtomicInteger cO;
    Semaphore SemH;
    Semaphore SemO;

    public Shared() {
       cH = new AtomicInteger();	// Inits variable in zero
       cO = new AtomicInteger();
       // creating a Semaphore object with number of permits 0
       SemH = new Semaphore(0);
       SemO = new Semaphore(0);
    }

    public int getH() {
       return cH.get();
    }

    public int getO() {
       return cO.get();
    }

    public void IncH() {
       cH.getAndIncrement();
    }

    public void DecH() {
       cH.getAndDecrement();
    }

    public void IncO() {
       cO.incrementAndGet();
    }

    public void DecO() {
       cO.decrementAndGet();
    }

    public void SignalH() {
       this.SemH.release();
    }

    public void SignalO() {
       this.SemO.release();
    }

    public void WaitH() {
       try {
          this.SemH.acquire();
       } catch ( InterruptedException e ) {
       }
    }

    public void WaitO() {
       try {
          this.SemO.acquire();
       } catch ( InterruptedException e ) {
       }
    }

}
  
class Particle extends Thread {
    int number;
    Shared resources;
    int particle;

    public Particle( int i, Shared res, int element ) {
       this.number = i;
       this.resources = res;
       this.particle = element;
    } 
  
    @Override
    public void run() {
       if ( 0 == this.particle ) {		// Create a new particle, hydrogen and oxygen
          System.out.println("Starting O thread " + this.number + " ... cO(" + resources.getO() + "), cH(" + resources.getH() + ")" );
          this.O();
       } else {
          System.out.println("Starting H thread " + this.number + " ... cO(" + resources.getO() + "), cH(" + resources.getH() + ")" );
          this.H();
       }

    }

    public void O() {
       if ( resources.getH() > 1 ) {
          System.out.println("\t O particle making water :) ... cO(" + resources.getO() + "), cH(" + resources.getH() + ")" );
          resources.DecH(); resources.DecH();
          resources.SignalH();
          resources.SignalH();
       } else {
          resources.IncO();
          resources.WaitO();
       }
    }

    public void H() {
       if ( (resources.getH() > 0) && (resources.getO() > 0) ) {
          System.out.println("\t H particle making water :) ... cO(" + resources.getO() + "), cH(" + resources.getH() + ")" );
          resources.DecH();
          resources.DecO();
          resources.SignalH();
          resources.SignalO();
       } else {
          resources.IncH();
          resources.WaitH();
       }
    }

} 
  
// Driver class 
public class Agua {
    public static void main(String args[]) throws InterruptedException {
        Random r;
        int element;
        Shared agua = new Shared();
        r = new Random();
        int threads = 102;	// To complete process need to be divisible by 3

        Thread particles[] = new Thread[ threads ];

        for ( int i = 0; i < threads; i++ ) {
           element = (r.nextInt() % 3) % 2;
// Create a new particle, randomly select hydrogen and oxygen
           particles[ i ]  = new Particle( i, agua, element );
           particles[ i ].start();
        }

        // waiting for threads
        for ( int i = 0; i < threads; i++ ) {
           try {
              particles[ i ].join();
           } catch ( InterruptedException e) {
           }
 
        }
    }

}

