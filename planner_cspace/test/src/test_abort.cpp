#include <actionlib/client/simple_action_client.h>
#include <gtest/gtest.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <planner_cspace/PlannerStatus.h>
#include <ros/ros.h>
#include <memory>

class AbortTest : public ::testing::Test {
 public:
  AbortTest()
      : node_() {
    move_base_ = std::make_shared<ActionClient>("/move_base");
    status_sub_ = node_.subscribe("/planner_3d/status",
                                  10,
                                  &AbortTest::StatusCallback,
                                  this);
    if (!move_base_->waitForServer(ros::Duration(30.0))) {
      ROS_ERROR("Failed to connect move_base action");
      exit(EXIT_FAILURE);
    }
  }
  ~AbortTest() {}

 protected:
  typedef actionlib::SimpleActionClient<
   move_base_msgs::MoveBaseAction> ActionClient;
  typedef std::shared_ptr<ActionClient> ActionClientPtr;

  void StatusCallback(const planner_cspace::PlannerStatus& msg) {
    status_ = msg;
  }
  move_base_msgs::MoveBaseGoal CreateGoalInRock() {
    move_base_msgs::MoveBaseGoal goal;
    goal.target_pose.header.stamp = ros::Time::now();
    goal.target_pose.header.frame_id = "map";
    goal.target_pose.pose.position.x = 1.19;
    goal.target_pose.pose.position.y = 1.90;
    goal.target_pose.pose.position.z = 0.0;
    goal.target_pose.pose.orientation.x = 0.0;
    goal.target_pose.pose.orientation.y = 0.0;
    goal.target_pose.pose.orientation.z = 0.0;
    goal.target_pose.pose.orientation.w = 1.0;
    return goal;
  }
  move_base_msgs::MoveBaseGoal CreateGoalInFree() {
    move_base_msgs::MoveBaseGoal goal;
    goal.target_pose.header.stamp = ros::Time::now();
    goal.target_pose.header.frame_id = "map";
    goal.target_pose.pose.position.x = 1.24;
    goal.target_pose.pose.position.y = 0.65;
    goal.target_pose.pose.position.z = 0.0;
    goal.target_pose.pose.orientation.x = 0.0;
    goal.target_pose.pose.orientation.y = 0.0;
    goal.target_pose.pose.orientation.z = 0.0;
    goal.target_pose.pose.orientation.w = 1.0;
    return goal;
  }
  
  ros::NodeHandle node_;
  ros::Subscriber status_sub_;
  ActionClientPtr move_base_;
  planner_cspace::PlannerStatus status_;
};


TEST_F(AbortTest, AbortByGoalInRock) {
  // Send a goal which is in Rock
  move_base_->sendGoal(CreateGoalInRock());
  while (move_base_->getState().state_ !=
         actionlib::SimpleClientGoalState::ACTIVE) {
    ros::Duration(1.0).sleep();
  }

  // Try to replan
  while (move_base_->getState().state_ ==
         actionlib::SimpleClientGoalState::ACTIVE) {
    ros::Duration(1.0).sleep();
  }

  // Abort after exceeding max_retry_num
  ASSERT_EQ(actionlib::SimpleClientGoalState::ABORTED,
            move_base_->getState().state_);
  ASSERT_EQ(planner_cspace::PlannerStatus::PATH_NOT_FOUND,
            status_.error);

  // Send another goal which is not in Rock
  move_base_->sendGoal(CreateGoalInFree());
  while (move_base_->getState().state_ !=
         actionlib::SimpleClientGoalState::ACTIVE) {
    ros::Duration(1.0).sleep();
  }
  while (move_base_->getState().state_ ==
         actionlib::SimpleClientGoalState::ACTIVE) {
    ros::Duration(1.0).sleep();
  }

  // Succeed
  ASSERT_EQ(actionlib::SimpleClientGoalState::SUCCEEDED,
            move_base_->getState().state_);
  ASSERT_EQ(planner_cspace::PlannerStatus::GOING_WELL,
            status_.error);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  ros::init(argc, argv, "test_abort");

  ros::AsyncSpinner spinner(1);
  spinner.start();
  int ret = RUN_ALL_TESTS();
  spinner.stop();
  ros::shutdown();
  return ret;
}
