"""
This module represents a device.

Computer Systems Architecture Course
Assignment 1
March 2016
"""
from threading import Event, Thread, Lock
from pool import WorkPool
from reusable_barrier import ReusableBarrier

class Device(object):
    """
    Class that represents a device.
    """

    def __init__(self, device_id, sensor_data, supervisor):
        """
        Constructor.

        @type device_id: Integer
        @param device_id: the unique id of this node; between 0 and N-1

        @type sensor_data: List of (Integer, Float)
        @param sensor_data: a list containing (location, data) as measured
                            by this device

        @type supervisor: Supervisor
        @param supervisor: the testing infrastructure's control and validation
                            component

        @type scripts_received: Event
        @param scripts_received: event which marks that all script in a
                                    timestamp were received

        @type scripts: List of Script
        @param scripts: list containing all the scripts received by a device

        @type thread: DeviceThread
        @param thread: thread associated with a device
        """
        self.device_id = device_id
        self.sensor_data = sensor_data
        self.supervisor = supervisor

        self.script_received = Event()
        self.scripts = []

        self.timepoint_done = Event()
        self.other_devices = []
        self.slock = Lock()

        self.barrier = None
        self.process = Event()

        self.global_thread_pool = None
        self.glocks = {}

        self.thread = DeviceThread(self)
        self.thread.start()

    def __str__(self):
        """
        Pretty prints this device. Because any device has the right to
        feel pretty.
        @rtype: String
        @return: a string containing the id of this device
        """
        return "Device %d" % self.device_id

    def setup_devices(self, devices):
        """
        Setup the devices before simulation begins.

        @type devices: List of Device
        @param devices: list containing all devices
        """
        # we don't need no stinkin' setup
        # right, but there is catch, without it we cant haz points
        self.other_devices = devices
        if self.device_id == self.other_devices[0].device_id:
            locks = {}
            for loc in self.sensor_data:
                locks[loc] = Lock()
            dev_cnt = len(devices)
            self.glocks = locks
            self.barrier = ReusableBarrier(dev_cnt)
            self.global_thread_pool = WorkPool(16)
        else:
            for loc in self.sensor_data:
                self.other_devices[0].glocks[loc] = Lock()
            self.glocks = self.other_devices[0].glocks
            self.global_thread_pool = self.other_devices[0].global_thread_pool
            self.barrier = self.other_devices[0].barrier

    def assign_script(self, script, location):
        """
        Provide a script for the device to execute.

        @type script: Script
        @param script: the script to execute from now on at each timepoint
        None if the current timepoint has ended

        @type location: Integer
        @param location: the location for which the script is interested in
        """
        if script is not None:
            self.scripts.append((script, location))
        else:
            # Dobby received a script, DOBBY IS FREE NOW !!!!!!!!
            self.script_received.set()

    def get_data(self, location):
        """
        Returns the pollution value this device has for the given location.

        @type location: Integer
        @param location: a location for which obtain the data

        @rtype: Float
        @return: the pollution value
        """
        # Shut up and show me your .. data
        ret = None
        with self.slock:
            if location in self.sensor_data:
                ret = self.sensor_data[location]
        return ret

    def set_data(self, location, data):
        """
        Sets the pollution value stored by this device for the given location.

        @type location: Integer
        @param location: a location for which to set the data

        @type data: Float
        @param data: the pollution value
        """
        with self.slock:
            if location in self.sensor_data:
                self.sensor_data[location] = data

    def shutdown(self):
        """
        Instructs the device to shutdown (terminate all threads). This method
        is invoked by the tester. This method must block until all the threads
        started by this device terminate.
        """
        self.thread.join()

class DeviceThread(Thread):
    """
    Thread that is dedicated to a specific device
    """
    def __init__(self, device):
        """
        Contructor
        """
        Thread.__init__(self, name="Device Thread %d" % device.device_id)
        self.device = device

    def run(self):
        """
        Wait untill all the neighbours and scripts are received an then send all
        the work to a common thread pool then wait untill the work is done.
        Because it's polite to wait after the one doing the work if we are not
        able to do it ourselves.
        """
        # hope there is only one timepoint, as multiple iterations of the
        # loop are not supported
        # we don't have to worry about those multiple iterations any more as
        # they are kind of supported now
        while True:
            # get the current neighbourhood
            neighbours = self.device.supervisor.get_neighbours()
            if neighbours is None:
                if self.device.device_id is self.device.other_devices[0].device_id:
                    self.device.global_thread_pool.end()
                break

            self.device.script_received.wait()

            # execute scripts received until now in parallel
            for (script, location) in self.device.scripts:
                self.device.global_thread_pool.work((self.device,
                									script,
                									location,
                									neighbours))

            self.device.global_thread_pool.finish_work()
            self.device.script_received.clear()
            self.device.barrier.wait()
