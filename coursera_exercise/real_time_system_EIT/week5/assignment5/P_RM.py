"""
Partitionned EDF using PartitionedScheduler.
"""
from simso.core.Scheduler import SchedulerInfo
from simso.utils import PartitionedScheduler
from simso.schedulers import scheduler
import math

@scheduler("simso.schedulers.P_RM")
class P_RM(PartitionedScheduler):
    def init(self):
        PartitionedScheduler.init(
            self, SchedulerInfo("simso.schedulers.RM_mono"))

    def packer(self):
        # First Fit
        cpus = [[cpu, 0] for cpu in self.processors]
        cpu_list = {}
        cpu_list[0] = 0
        cpu_list[1] = 0
        cpu_list[2] = 0
        for task in self.task_list:
            m = cpus[0][1]
            j = 0
            print task.period
            # Find the processor with the lowest load.
            for i, c in enumerate(cpus):
                util = c[1] + float(task.wcet) / task.period
                task_num = cpu_list[i] + 1
                urm = task_num*(math.pow(2, 1.0/task_num) - 1)
                print i, util, task_num, urm
                if util < urm:
                    print task.period
                    self.affect_task_to_processor(task, cpus[i][0])
                    cpus[i][1] += float(task.wcet) / task.period
                    cpu_list[i] += 1
                    break
                else:
                    continue
                #if c[1] < m:
                    #m = c[1]
                    #j = i

            # Affect it to the task.
            #self.affect_task_to_processor(task, cpus[j][0])

            # Update utilization.
            #cpus[j][1] += float(task.wcet) / task.period
        return True
