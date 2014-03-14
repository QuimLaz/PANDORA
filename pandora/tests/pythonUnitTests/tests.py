#!/usr/bin/python3

# unit testing of pyPandora

import sys, random
import unittest
sys.path.append('../../')
sys.path.append('../../pyPandora')

from pyPandora import Simulation, Agent, World, Point2DInt, SizeInt, RectangleInt

class TestAgent(Agent):
	def __init__(self, id):
		Agent.__init__( self, id)

	def updateState(self):
		return

	def serialize(self):
		return

class TestWorld(World):
	def __init__(self, simulation, worldOfLastTest = False ):
		World.__init__( self, simulation)
		if not worldOfLastTest:
			self.setFinalize(False)

	def createRasters(self):
		return

	def createAgents(self):
		return

class TestPyPandora(unittest.TestCase):
	
	def setUp(self):
		return
	
	def testEqualityPoint(self):
		point1 = Point2DInt(4,5)
		point2 = Point2DInt(2,5)
	
		self.assertNotEqual(point1, point2)

		point2._x = 4
		self.assertEqual(point1, point2)

		point2._y = 100
		self.assertNotEqual(point1, point2)

	def testSimulationSize(self):
		mySimulation = Simulation(SizeInt(10,10), 1)
		size = SizeInt(9,10)

		self.assertNotEqual(mySimulation.size, size)
		size._width = 10
		self.assertEqual(mySimulation.size, size)

	def testAgentRemovedIsNotExecuted(self):
		mySimulation = Simulation(SizeInt(10,10), 1)
		myWorld = TestWorld(mySimulation)
		myWorld.initialize()

		myAgent = TestAgent('agent_0')
		myWorld.addAgent(myAgent)
		myAgent.setRandomPosition()
		self.assertEqual(myAgent.exists, True)
		myAgent.remove()
		self.assertEqual(myAgent.exists, False)
		myWorld.run()
	
	def testExecuteWorldTwice(self):
		mySimulation = Simulation(SizeInt(10,10), 1)
		myWorld = TestWorld(mySimulation, False)
		myWorld.initialize()
		myWorld.run()
		myWorld.setFinalize(True)
		myWorld.initialize()
		myWorld.run()
	
	def testAgentRemovedIsNotInsideNeighbours(self):
		mySimulation = Simulation(SizeInt(10,10), 1)
		myWorld = TestWorld(mySimulation)
		myWorld.initialize()
		myWorld.run()

		myAgent0 = TestAgent('agent_0')
		myAgent1 = TestAgent('agent_1')
		myAgent2 = TestAgent('agent_2')
		myWorld.addAgent(myAgent0)
		myWorld.addAgent(myAgent1)
		myWorld.addAgent(myAgent2)
		myAgent0.setRandomPosition()
		myAgent1.setRandomPosition()
		myAgent2.setRandomPosition()

		agentIds = myWorld.getNeighboursIds(myAgent0, 20, 'agent')
		self.assertEqual(len(agentIds), 2)
		myAgent1.remove()
		myAgent2.remove()
		agentIds = myWorld.getNeighboursIds(myAgent0, 20, 'agent')
		self.assertEqual(len(agentIds), 0)
    	
	def testRectangleEquals(self):
		aRectangle = RectangleInt.fromCoordinates(5,10,20,30);
		bRectangle = RectangleInt(Point2DInt(3,7), SizeInt(1,1))

		self.assertNotEqual(aRectangle, bRectangle)

		bRectangle._origin = Point2DInt(5,10)
		self.assertNotEqual(aRectangle, bRectangle)

		bRectangle._size = SizeInt(16,21)
		self.assertEqual(aRectangle, bRectangle)

	def testRectangleContainsPoint(self):
		aRectangle = RectangleInt.fromCoordinates(0,0,20,10);
		aPoint = Point2DInt(-1,-1)
		
		self.assertFalse(aRectangle.contains(aPoint))

		aPoint._x = 0		
		self.assertFalse(aRectangle.contains(aPoint))

		aPoint._y = 0
		self.assertTrue(aRectangle.contains(aPoint))

		aPoint._x = 20
		self.assertTrue(aRectangle.contains(aPoint))

		aPoint._y = 10
		self.assertTrue(aRectangle.contains(aPoint))

		aPoint._x = 21
		self.assertFalse(aRectangle.contains(aPoint))
   
if __name__ == '__main__':
    unittest.main()