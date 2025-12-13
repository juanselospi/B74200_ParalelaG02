/**
 * La clase PiPorSerieThreads lleva a cabo calculos de aproximación de PI usando threads
 *
 * @author Programacion Concurrente
 * @version 2019/Ago/28
 */

/**
 * Esta clase Suma es necesaria para obtener los valores de la suma de los terminos de PI
 * que realiza cada hilo
 * En Java, los metodos a los que se permite hacer fork son void y no pueden retornar resultados,
 * tampoco pueden recibir parametros
 *
 * Cada hilo crea una instancia de esta clase para almacenar la suma de los terminos que le
 * toco calcular
 */
class Suma {
    private double suma;

    public Suma() {
       this.suma = 0;
    }

    public void setSuma( double suma ) {
       this.suma = suma;
    }

    public double getSuma() {
       return this.suma;
    }

}

/**
 * Esta es la clase que vamos a correr cada vez que se crea un hilo
 * Se utiliza el metodo "run" para realizar la tarea de cada hilo
 *
 */
class SeriePorPartes implements Runnable {
    private long fin;
    private long inicio;
    private Suma miParte;

/**
 * En el constructor se definen los limites para los que este hilo realiza
 * el calculo de terminos
 * @param	long	inicio, es el punto donde inicia la suma que calcula este hilo
 * @param	long	fin, es la cantidad de terminos que le toca calcular a este hilo
 * @param	Suma	miParte, una referencia de Suma para guardar el resultado de este hilo
 */
    public SeriePorPartes( long inicio, long fin, Suma miParte ) {
        this.inicio = inicio;
        this.fin = fin;
        this.miParte = miParte;
    }

/**
 * Este metodo es el que ejecuta cada hilo que se crea
 * Realiza la suma de los terminos que van desde el valor "inicial"
 * hasta la cantidad de "terminos"
 * Coloca el resultado en una instancia de la clase Suma
 */
    public void run() {
       double miSuma = 0;
       double alterna;

       alterna = ( 0 == ( inicio % 2 ) )? 4 : -4;
       for ( long i = inicio; i < fin; i++ ) {
           miSuma += alterna/(2 * i + 1);
           alterna *= -1;
       }
       miParte.setSuma( miSuma );
    }

}

public class PiPorSerieThreads {


/**
 * Realiza las pruebas de la clase
 */
    public static void main( String[] args ) {
        long n;
        double x = 0;
        Suma miParte[]  = new Suma[ 10 ];	// Una instancia para guardar los resultados de cada hilo
        Thread thread[] = new Thread[ 10 ];	// Una instancia por cada hilo creado
						// Se utilizan para esperar por las sumas de cada hilo

        n = Long.parseLong( args[ 0 ] );
        for ( int i = 0; i < 10; i++ ) {	// Generar los 10 hilos calculadores
           miParte[ i ] = new Suma();
           long inicio = i * n / 10;
           long fin = (i+1)*n/10;
           thread[ i ] = new Thread( new SeriePorPartes( inicio, fin, miParte[ i ] ) );
           thread[ i ].start();
        }

        for ( int i = 0; i < 10; i++ ) {	// Esperar por los resultados de cada hilo
           try {
              thread[ i ].join();
              x += miParte[ i ].getSuma();
//    System.out.println("Valor aportado por hilo " + i + " es " + miParte[ i ].getSuma() );
           } catch ( InterruptedException e) {
           }
        }
        System.out.println( "Valor calculado de PI: " + x );

    }
}

