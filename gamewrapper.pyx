cdef extern void initSensor()
cdef extern void freeSensor()
cdef extern void setTX()
cdef extern void clearTX()
cdef extern int readRX()
cdef extern void initTimer()
cdef extern void freeTimer()
cdef extern unsigned long long getSystemTimerCounter() 

#code taken from tutorial 5 us100,added timer class from lecture about RPi microsecond timer
class Sensor: 
	def __init__(self): 
		initSensor()
		print("Initialization of GPIO successful")
		
	def __del__(self):
		freeSensor()
		
	def TX_set(self):
		setTX()
	
	def TX_clear(self):
		clearTX()
		
	def RX_read(self):
		return readRX()
		
	
class Timer : 
	def __init__(self): 
		initTimer() 
	
	def __del__(self): 
		freeTimer()
	
	def getTimer(self): 
		return getSystemTimerCounter() 
		
