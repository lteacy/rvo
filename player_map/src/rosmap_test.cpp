#include "gtest/gtest.h"

#include <player_map/rosmap.hpp>

#include <Eigen/Dense>

using namespace Eigen;
using namespace rf;
enum {
  FREE = -1,
  UNKNOWN = 0,
  OCCUPIED = 1
};

class DijkstraTest : public testing::Test {
protected:
  void SetUp() {
    // Make 3x3 completely free map centered at origin
    Map3x3_ = map_alloc();
    int width = 3, height = 3;
    Map3x3_->origin_x = 0.0;
    Map3x3_->origin_y = 0.0;
    Map3x3_->size_x = width;
    Map3x3_->size_y = height;
    Map3x3_->scale = 1.0;
    Map3x3_->cells = (map_cell_t*) calloc(width * height, sizeof(Map3x3_->cells[0]));
    for (int x = -1; x < 2; ++x) {
      for (int y = -1; y < 2; ++y) {
        map_get_cell(Map3x3_, x, y, 0)->occ_state = FREE;
      }
    }

    // Make 100x100 completely free map centered at origin
    Map100x100_ = map_alloc();
    width = 100, height = 100;
    Map100x100_->origin_x = 0.0;
    Map100x100_->origin_y = 0.0;
    Map100x100_->size_x = width;
    Map100x100_->size_y = height;
    Map100x100_->scale = 1.0;
    Map100x100_->cells = (map_cell_t*) calloc(width * height, sizeof(Map100x100_->cells[0]));
    for (int x = -50; x < 50; ++x) {
      for (int y = -50; y < 50; ++y) {
        map_get_cell(Map100x100_, x, y, 0)->occ_state = FREE;
      }
    }
    
  }

  void TearDown() {
    map_free(Map3x3_);
    map_free(Map100x100_);    
  }
  map_t *Map3x3_, *Map100x100_;
};

TEST_F(DijkstraTest, OneStepRight) {
  Vector2f start(0, 0), stop(0, 1);
  PointVector path = dijkstra(start, stop, Map3x3_);
  ASSERT_EQ(path.size(), (unsigned)2);
  EXPECT_EQ(path[0], start);
  EXPECT_EQ(path[1], stop);  
}

TEST_F(DijkstraTest, OneStepDiag) {
  Vector2f start(0, 0), stop(1, 1);
  PointVector path = dijkstra(start, stop, Map3x3_);
  ASSERT_EQ(path.size(), (unsigned)2);
  EXPECT_EQ(path[0], start);  
  EXPECT_EQ(path[1], stop);
}

TEST_F(DijkstraTest, TwoSteps) {
  Vector2f start(-1, -1), stop(1, 1);
  PointVector path = dijkstra(start, stop, Map3x3_);
  ASSERT_EQ(path.size(), (unsigned)3);
  EXPECT_EQ(path[0], start);
  EXPECT_EQ(path[1], Vector2f(0, 0));
  EXPECT_EQ(path[2], stop);
}

TEST_F(DijkstraTest, Obstacles) {
  Vector2f start(-1, -1), stop(1, -1);
  map_get_cell(Map3x3_, 0.0, 0.0, 0)->occ_state = OCCUPIED;
  map_get_cell(Map3x3_, 0.0, -1.0, 0)->occ_state = OCCUPIED;
  
  PointVector path = dijkstra(start, stop, Map3x3_);
  ASSERT_EQ(path.size(), (unsigned)5);
  EXPECT_EQ(path[0], start);  
  EXPECT_EQ(path[1], Vector2f(-1, 0));
  EXPECT_EQ(path[2], Vector2f(0, 1));  
  EXPECT_EQ(path[3], Vector2f(1, 0));
  EXPECT_EQ(path[4], Vector2f(1, -1));
}

TEST_F(DijkstraTest, BiggerMap) {
  Vector2f start(-50, -50), stop(49, 49);
  
  PointVector path = dijkstra(start, stop, Map100x100_);
  ASSERT_EQ(path.size(), (unsigned)100);
}

class SightTest : public testing::Test {
protected:
  void SetUp() {
    // Make 3x3 completely free map centered at origin
    Map3x3_ = map_alloc();
    int width = 3, height = 3;
    Map3x3_->origin_x = 0.0;
    Map3x3_->origin_y = 0.0;
    Map3x3_->size_x = width;
    Map3x3_->size_y = height;
    Map3x3_->scale = 1.0;
    Map3x3_->cells = (map_cell_t*) calloc(width * height, sizeof(Map3x3_->cells[0]));
    for (int x = -1; x < 2; ++x) {
      for (int y = -1; y < 2; ++y) {
        map_get_cell(Map3x3_, x, y, 0)->occ_state = FREE;
      }
    }

    // Make 100x100 completely free map centered at origin
    Map100x100_ = map_alloc();
    width = 100, height = 100;
    Map100x100_->origin_x = 0.0;
    Map100x100_->origin_y = 0.0;
    Map100x100_->size_x = width;
    Map100x100_->size_y = height;
    Map100x100_->scale = 1.0;
    Map100x100_->cells = (map_cell_t*) calloc(width * height, sizeof(Map100x100_->cells[0]));
    for (int x = -50; x < 50; ++x) {
      for (int y = -50; y < 50; ++y) {
        map_get_cell(Map100x100_, x, y, 0)->occ_state = FREE;
      }
    }
    
  }

  void TearDown() {
    map_free(Map3x3_);
    map_free(Map100x100_);    
  }
  map_t *Map3x3_, *Map100x100_;
};

TEST_F(SightTest, AllLOS) {  
  LOSChecker c(Map3x3_);
  for (double x1 = -1.4; x1 < 1.5; x1 += 0.25) {
    for (double y1 = -1.4; y1 < 1.5; y1 += 0.25) {
      for (double x2 = -1.4; x2 < 1.5; x2 += 0.25) {
        for (double y2 = -1.4; y2 < 1.5; y2 += 0.25) {
          // fprintf(stderr, "%f %f %f %f\n", x1, y1, x2, y2);
          ASSERT_TRUE(c.LineOfSight(x1, y1, x2, y2));
        }
      }
    }
  }
}

TEST_F(SightTest, MiddleBlocked) {
  LOSChecker c(Map3x3_);  
  map_get_cell(Map3x3_, 0, 0, 0)->occ_state = OCCUPIED;
  ASSERT_TRUE(c.LineOfSight(-1, 1, 0, 1));
  ASSERT_TRUE(c.LineOfSight(-1, 1, 1, 1));
  
  ASSERT_FALSE(c.LineOfSight(-1, 0, 1, 1));
  ASSERT_FALSE(c.LineOfSight(-1, 0, 1, 0));
  ASSERT_FALSE(c.LineOfSight(-1, 0, 1, -1));

  ASSERT_TRUE(c.LineOfSight(0.6, -0.2, 0.6, 0.6));
  ASSERT_FALSE(c.LineOfSight(0.6, -0.2, 0, 0.6));
}

TEST_F(SightTest, BigMap) {
  LOSChecker c(Map100x100_);
  for (double y = -50; y < 49.5; y += 0.25) {  
    map_get_cell(Map100x100_, 0, y, 0)->occ_state = OCCUPIED;
  }

  for (double x = -50.0; x < -1.0; x += 1.0) {
    for (double y = -50; y < 49.5; y += 0.25) {
      ASSERT_FALSE(c.LineOfSight(x, y, 21.25, 3.25));
    }
  }
}

int main(int argc, char **argv){
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

