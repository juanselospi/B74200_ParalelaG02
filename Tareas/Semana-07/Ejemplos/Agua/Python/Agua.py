import threading
import sys
import time
import random

"""
   Shared class contains resources to resolv H2O problem
   It has
      - two integers to count hydrogen and oxygen particles
      - two locks to make all integers operations atomic
         - return, increment, and decrement are executed atomically using these locks
      - two semaphores to stop particles when it is not possible to make water
"""
class Shared:

	def __init__( self ):	# Constructor
		self.cH = 0
		self.cO = 0
		self.SemH = threading.Semaphore( 0 )
		self.SemO = threading.Semaphore( 0 )
		self.lockH = threading.Lock()
		self.lockO = threading.Lock()

	def getH( self ):
		with self.lockH:	# Only one thread can pass this lock
			return self.cH

	def getO( self ):
		with self.lockO:
			return self.cO

	def decH( self ):
		with self.lockH:
			self.cH -= 1

	def decO( self ):
		with self.lockO:
			self.cO -= 1

	def incH( self ):
		with self.lockH:
			self.cH += 1

	def incO( self ):
		with self.lockO:
			self.cO += 1

	def signalH( self ):
		self.SemH.release()

	def signalO( self ):
		self.SemO.release()

	def waitH( self ):
		self.SemH.acquire()

	def waitO( self ):
		self.SemO.acquire()


"""
   Agua is where all particles are created using a thread for each one
   It is an extension for the threading.Thread class and we need to overcharge constructor and run methods
"""
class Agua ( threading.Thread ):

	def __init__( self, threadID, resources ):
		threading.Thread.__init__( self )
		self.threadID = threadID
		self.resources = resources

	def run(self):	# Create randomly hydrogens and oxygens
		r = random.randint( 1, 10 )
		if ( 0 == (r % 2) ):
			print( "Starting O thread " + str( self.threadID ) + " ... cO(" + str(self.resources.getO()) + "), cH(" + str(self.resources.getH()) + ")" )
			self.O()
		else:
			print( "Starting O thread " + str( self.threadID ) + " ... cO(" + str(self.resources.getO()) + "), cH(" + str(self.resources.getH()) + ")" )
			self.H()

	def H( self ):	# Represent an hydrogen particle
		if ( (self.resources.getO() > 0) and (self.resources.getH() > 0) ):
			print( "\tH particle making water :) ... " + str( self.threadID ) + " ... cO(" + str(self.resources.getO()) + "), cH(" + str(self.resources.getH()) + ")" )
			self.resources.decH()
			self.resources.decO()
			self.resources.signalO()
			self.resources.signalH()
		else:
			self.resources.incH()
			self.resources.waitH()

	def O( self ):	# Represent an oxygen particle
		if ( self.resources.getH() > 1 ):
			print(  "\tO particle making water :) ... " + str( self.threadID ) + " ... cO(" + str(self.resources.getO()) + "), cH(" + str(self.resources.getH()) + ")" )
			self.resources.decH()
			self.resources.decH()
			self.resources.signalH()
			self.resources.signalH()
		else:
			self.resources.incO()
			self.resources.waitO()

def main():
	threads = []
	resources = Shared();

	for i in range( 100 ):
		t = Agua( i, resources )
		threads.append( t )
		t.start()

	for t in threads:
		t.join()

if __name__ == "__main__":
    main()

