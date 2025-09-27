import sys
import threading

class valores:
	pass

def calcula( inicio, terminos, total ):
	miSuma=0.0
	alterna = 4.0 if ( 0 == (inicio % 2) ) else -4.0
	for i in range( terminos ):
		pos = i + inicio
		miSuma += alterna/(2 * pos + 1)
		alterna *= -1.0
	total.suma += miSuma


def main():
	threads = []
	valor = valores
	valor.suma = 0
	terminos = 1000000
	if ( len( sys.argv ) > 1 ):
		terminos = long( sys.argv[1] )
	else:
		print( "Uso: " + '\033[1m' + sys.argv[ 0 ] + " n (para n un entero)" + '\033[0m')
	fin = terminos//10
	x = float( 0 )
	for i in range( 10 ):
		inicio = i * terminos/10
		t = threading.Thread( target=calcula, args=( inicio, fin, valor ) )
		threads.append( t )
		t.start()

	for t in threads:
		t.join()

	print( "Valor aproximado de PI: " + '\033[91m' + str( valor.suma ) + '\033[0m' + " para " + str( terminos ) + " elementos")


if __name__ == "__main__":
	main()

