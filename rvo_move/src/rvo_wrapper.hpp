#ifndef RVO_WRAPPER_HPP
#define RVO_WRAPPER_HPP

#include <ros/ros.h>

#include <actionlib/server/simple_action_server.h>
#include <actionlib/client/simple_action_client.h>

#include <rvo_move/MoveAction.h>

#include <RVO/RVO.h>

#include <player_map/rosmap.hpp>

#include <rvo_move/bot_client.hpp>

namespace rf {
  class RVOWrapper {
  public:
    RVOWrapper(std::vector<BotClient*> bots, size_t id, map_t *map,
               const std::vector<std::vector<RVO::Vector2> > &obstacles);
    ~RVOWrapper();

    static RVOWrapper* ROSInit(const ros::NodeHandle& nh, map_t *map, std::vector<BotClient*> bots);
    
    bool step();
    std::vector<geometry_msgs::Pose> setGoal(const geometry_msgs::Pose& p);

    size_t getID() const { return id_; }
    bool syncState();
    bool setVelocities();

    void setAgentDefaults(float neighborDist, size_t maxNeighbors, float timeHorizon,
                          float timeHorizonObst, float radius, float maxSpeed);
    void setTimestep(float ts) { timestep_ = ts; }
    void setAxelWidth(float width) { axel_width_ = width; }
    void setWaypointSpacing(float spacing) { way_spacing_ = spacing; }
    void setPathMargin(float margin) { path_margin_ = margin; }
    void setGoalTolerance(float tol) { goal_tol_ = tol; }
    void setLOSMargin(float marg) { los_margin_ = marg; }
    void addAgents();
  private:
    bool getLeadGoal(RVO::Vector2 *goal);
    bool getOtherGoal(int bot_id, RVO::Vector2 *goal);
    
    RVO::RVOSimulator *sim_;
    std::vector<rf::BotClient *> bots_;
    RVO::Vector2 goal_;
    std::vector<RVO::Vector2> waypoints_;
    map_t *map_;
    LOSChecker *checker_;
    size_t id_;
    
    float max_dist_;
    float axel_width_;
    float goal_tol_;
    float path_margin_;
    float way_spacing_;
    float timestep_;
    float los_margin_;
  };

  class MoveServer {
  public:
    MoveServer(const std::string &server_name);
    ~MoveServer();
    void executeCB(const rvo_move::MoveGoalConstPtr &goal);
    void start();
  private:
    ros::NodeHandle nh_, pnh_;
    ros::Publisher path_pub_;
    actionlib::SimpleActionServer<rvo_move::MoveAction> as_;
    std::string action_name_;
    std::string tf_frame_;
    std::vector<BotClient *> bots_;
    map_t *map_;
    rf::RVOWrapper *wrapper_;
    double timestep_;
  };
}
#endif
