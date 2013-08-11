import os;
import ah_config;

#queues = ah_config.queues
#hasPartials = ah_config.hasPartials
pParameter = ah_config.pParameter
executables = ah_config.executables 

#works = ah_config.works
#threads = ah_config.threads
#maxThreads = ah_config.maxThreads

def runProdcon(queues, works, threads, maxThreads, prefill = 0, directory='', performance = True):

  perfParam = '-log_operations -noprint_summary'
  if performance:
    perfParam = '-nolog_operations -print_summary'


  for queue in queues:
      
      for work in works:

          for thread in threads:

              exe = queue
              if queue in executables:
                exe = executables[queue]

              template = "../prodcon-{exe} -producers {thread} -consumers {thread} -operations 10000 -c {work} {partials_param} {partials} {perfParam} -noset_rt_priority > {directory}{queue}-t{thread}{partials_param}{partials}-c{work}.txt"

              commands = []
              if queue in pParameter:
                  commands = []
#                  commands = [template.format(exe=exe,
#                                              queue=queue,
#                                              thread=thread, 
#                                              work=work, 
#                                              partials_param='-' + pParameter[queue], 
#                                              partials=str(thread*2),
#                                              directory=directory)]

#                  if 2*thread < maxThreads:
                  commands.extend([template.format(queue=queue,
                                                   exe=exe,
                                                   thread=thread, 
                                                   work=work, 
                                                   partials_param='-' + pParameter[queue], 
                                                   partials=maxThreads,
                                                   directory=directory,
                                                   perfParam=perfParam,
                                                   prefill = str(prefill))])
              else:

                  commands = [template.format(queue=queue,
                                              exe = exe,
                                              thread=thread, 
                                              work=work, 
                                              partials_param='', 
                                              partials='',
                                              directory=directory,
                                              perfParam = perfParam,
                                              prefill = str(prefill))]

              for command in commands:
                  print command
                  os.system(command)

#runSeqalt(queues = ah_config.queues, works = ah_config.works, threads = ah_config.threads, maxThreads = ah_config.maxThreads)
