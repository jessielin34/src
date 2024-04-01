# This is a python launch file to control and launch ros nodes for Semaforr project
# Each ros node must has a launch file containing parameters that remain constant though different experiments
# Dynamic parameters that changes with different experiments are added here

import rospy
import time
import subprocess
import os

def experiment(map_name, log_name, density, flow, risk, cusum, discount, explore, advisors, params):
    project_home = os.path.expanduser("~/catkin_ws/src")
    menge_path = project_home+"/examples/core"
    semaforr_path = project_home+"/semaforr"

    map_folder = menge_path+"/"+map_name
    map_xml = menge_path+"/"+map_name+".xml"

    #menge files for semaforr
    map_config = map_folder+"/"+map_name+"S.xml"
    map_dimensions = map_folder+"/dimensions.conf"
    target_set = map_folder+"/" + target_file_name


    print(target_set)
    print(map_config)
    print(map_xml)
    print(map_dimensions)
    print(log_name)
    print(why_log_name)
    print(whyplan_log_name)
    #print(situation_log_name)

    #start roscore
    roscore = subprocess.Popen(['roscore'])
    time.sleep(5)

    # start menge simulator
    # menge_sim_process = subprocess.Popen(['rosrun','menge_sim','menge_sim','-p',map_xml])
    # print("waiting,,")
    # time.sleep(30)

    # start crowd model
    #crowd_process = subprocess.Popen(['rosrun','crowd/crowd_learner','learn.py',density, flow, risk, cusum, discount, explore])

    # start logging
    log_file = open(log_name,"w")
    log_process = subprocess.Popen(['rostopic','echo','/decision_log'],stdout=log_file)

    # why_explanations_file = open(why_explanations_name,"w")
    # why_explanations_process = subprocess.Popen(['rostopic','echo','/explanations'],stdout=why_explanations_file)
    # whyplan_explanations_file = open(whyplan_explanations_name,"w")
    # whyplan_explanations_process = subprocess.Popen(['rostopic','echo','/plan_explanations'],stdout=whyplan_explanations_file)

    # why_log_file = open(why_log_name,"w")
    # why_log_process = subprocess.Popen(['rostopic','echo','/explanations_log'],stdout=why_log_file)
    # whyplan_log_file = open(whyplan_log_name,"w")
    # whyplan_log_process = subprocess.Popen(['rostopic','echo','/plan_explanations_log'],stdout=whyplan_log_file)

    # start semaforr
    semaforr_process = subprocess.Popen(['rosrun','semaforr','semaforr', semaforr_path, target_set, map_config, map_dimensions, advisors, params])
    print("waiting,,")
    time.sleep(2)
    
    # start why
    # why_process = subprocess.Popen(['rosrun','why','why'])
    # print "waiting,,"

    # start why_plan
    # why_plan_process = subprocess.Popen(['rosrun','why_plan','why_plan'])
    # print "waiting,,"
   
    rviz_process = subprocess.Popen(['rosrun','rviz','rviz'])

    # Wait till semaforr completes the process
    while semaforr_process.poll() is None:
        print("Semaforr process still running ...")
        if rviz_process.poll() is not None:
            rviz_process = subprocess.Popen(['rosrun','rviz','rviz'])
        # if menge_sim_process.poll() is not None or str(subprocess.check_output(["ps -A | grep 'menge' | wc -l"],shell=True))[0] != "1":
        #     break
        time.sleep(1)
    try:
        semaforr_process.terminate()
        while semaforr_process.poll() is None:
            print("Semaforr process still running ...")
            time.sleep(1)
    except:
        print("Semaforr already terminated")
    print("Semaforr process has ended ...")
    print("Terminating the simulator")

    # try:
    #     menge_sim_process.terminate()
    #     while menge_sim_process.poll() is None:
    #         print("Menge process still running ...")
    #         time.sleep(1)
    # except:
    #     print("Menge already terminated")
    # print("Menge terminated!")

    rviz_process.terminate()
    
    # print "Terminating crowd model"
    #crowd_process.terminate()
    # why_process.terminate()
    # why_plan_process.terminate()
    # print "Why terminated!"
    log_process.terminate()
    log_file.close()
    # why_explanations_process.terminate()
    # why_explanations_file.close()
    # whyplan_explanations_process.terminate()
    # whyplan_explanations_file.close()
    # why_log_process.terminate()
    # why_log_file.close()
    # whyplan_log_process.terminate()
    # whyplan_log_file.close()
    time.sleep(1)

    roscore.terminate()
    time.sleep(30)
    print("roscore terminated!")

density = "off"
flow = "off"
risk = "off"
cusum = "off"
discount = "off"
explore = "off"

num_runs = 1
advisors = "/config/advisors.conf"
params = "/config/params.conf"
map_name = "gradcenter-5"
for i in range(0,num_runs):
    for j in range(1,6):
        why_explanations_name = map_name + "_" + str(j) + "_" + str(i) + "_why_explanations.txt"
        whyplan_explanations_name = map_name + "_" + str(j) + "_" + str(i) + "_why_plan_explanations.txt"
        why_log_name = map_name + "_" + str(j) + "_" + str(i) + "_why_log.txt"
        whyplan_log_name = map_name + "_" + str(j) + "_" + str(i) + "_why_plan_log.txt"
        log_name = map_name + "log_" + str(j) + "_" + str(i) + ".txt"
        target_file_name = "target40test-" + str(j) + ".conf"
        experiment(map_name, log_name, density, flow, risk, cusum, discount, explore, advisors, params)