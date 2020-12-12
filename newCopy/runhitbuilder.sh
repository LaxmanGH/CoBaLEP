#!/bin/bash

# Lines beginning with the # symbol are comments and are not interpreted by 
# the Job Scheduler.

# Lines beginning with #SBATCH are special commands to configure the job.
		
### Job Configuration Starts Here #############################################

# Export all current environment variables to the job (Don't change this)
#SBATCH --get-user-env 
			
# The default is one task per node
#SBATCH --ntasks=1
#SBATCH --nodes=1

#request 100 minutes of runtime - the job will be killed if it exceeds this
#SBATCH --time=100:00

# Change email@example.com to your real email address
##SBATCH --mail-user=CJ.Barton@coyotes.usd.edu
##SBATCH --mail-type=ALL



### Commands to run your program start here ####################################
root -q -b "hitbuilder2020.C(\"$counter\")"
pwd

