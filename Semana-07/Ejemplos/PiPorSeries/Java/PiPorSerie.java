/**
 * La clase PiPorSerie lleva a cabo calculos de aproximación 
 *
 * @author Programacion Concurrente
 * @version 2019/Ago/28
 */

public class PiPorSerie {

    private long terminos;
    private long inicial;

/**
 * Constructor
 */
    public PiPorSerie( long terminos ) {
       this.terminos = terminos;
       this.inicial = 0;
    }

/**
 * Constructor
 */
    public PiPorSerie( long inicial, long terminos ) {
       this.terminos = terminos;
       this.inicial = inicial;
    }


/**
 * Realiza la operacion para el calculo de PI por medio de la serie 4/1 -4/3 + 4/5 - 4/7 ...
 * @return	double		devuelve el valor aproximado de PI
 */
    public double aproximacionPI( long inicial, long terminos ) {
       double casiPI = 0;
       double alterna = 4;

       for ( long i = inicial; i < terminos; i++ ) {
           casiPI += alterna/(2 * i + 1);
           alterna *= -1;
       }
       return casiPI;
    }

/**
 * Realiza las pruebas de la clase
**/
    public static void main( String[] args ) {
        long n;
        double x;

        n = Long.parseLong( args[ 0 ] );
        PiPorSerie pi = new PiPorSerie( n );
        x = pi.aproximacionPI( 0, n );
        System.out.println( "Valor calculado de PI: " + x );

    }
}

