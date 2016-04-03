
#ifndef vision_h
#define vision_h

#include <pcl/ModelCoefficients.h>
#include <pcl/point_types.h>
#include <pcl/sample_consensus/method_types.h>
#include <pcl/sample_consensus/model_types.h>
#include <pcl/segmentation/sac_segmentation.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/segmentation/region_growing_rgb.h>
#include <pcl/search/search.h>
#include <pcl/search/kdtree.h>
#include <pcl/filters/passthrough.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/filters/statistical_outlier_removal.h>

int colored_segmentation(const sensor_msgs::PointCloud2ConstPtr& cloud_data);
void show_occupied_cells(int CELL_TYPE);

const int USER = 0;
const int COMP = 1; 

int PLAYER = USER;

struct coordinates{
double min_x;
double max_x;
double min_y;
double max_y;
};


void work(const sensor_msgs::PointCloud2ConstPtr& cloud_data){

  /*
  int user_move =  colored_segmentation(cloud_data);  
  //cout << "move : " << user_move << endl;
  //cin.get();
  return user_move;
  */
  
  disp_matrix();
  cout << "Play : USER (press enter ) " << endl;
  
    cin.get();
  int user_move = colored_segmentation(cloud_data);

  // if user move seen, do computer move
  if(user_move){
    cout << "user moved : " << user_move << endl;
    
    get_player_move(user_move);
    done = check();

    int comp_move = get_computer_move();
    cout << "moving to cell : " << comp_move << endl;
    move_it(comp_move);
    CELL_OCCUPIED[comp_move] = true;
    done = check();
  }

  if(done != ' '){
    if(done == 'X')
      cout << "user won" << endl;
    else
      cout << "comp won" << endl;
    ros::shutdown();
  }

}




int get_row(double value){
  	coordinates row_range[3];
	row_range[0].min_y = -0.1;
	row_range[0].max_y = -0.052;

	row_range[1].min_y = -0.050;
	row_range[1].max_y = -0.003;

	row_range[2].min_y = -0.001;
	row_range[2].max_y = 0.05;
	
	// check row 1
	if(value > row_range[0].min_y && value < row_range[0].max_y)
	  return 1;
	// check row 2
	if(value > row_range[1].min_y && value < row_range[1].max_y)
	  return 2;
	// check row 3
	if(value > row_range[2].min_y && value < row_range[2].max_y)
	  return 3;

}

int get_col(double value){
  coordinates col_range[3];
  col_range[0].min_x = -0.18;
  col_range[0].max_x = -0.1;
  
  col_range[1].min_x = -0.085;
  col_range[1].max_x = -0.022;
  
  col_range[2].min_x = 0;
  col_range[2].max_x = 0.066;

  // check col 1
  if(value >= col_range[0].min_x && value <= col_range[0].max_x)
    return 1;
  // check col 2
  if(value >= col_range[1].min_x && value <= col_range[1].max_x)
    return 2;
  // check col 3
  if(value >= col_range[2].min_x && value <= col_range[2].max_x)
    return 3;
}

int get_cell(int r,int c){
  switch(r){
  case 1:{
    if(c == 1)
      return 1;
    else if(c == 2)
      return 2;
    else return 3;
  }break;

  case 2:{
    if(c == 1)
      return 4;
    else if(c == 2)
      return 5;
    else return 6;
  }break;

  case 3:{
    if(c == 1)
      return 7;
    else if(c == 2)
      return 8;
    else return 9;
  }break;

  default:break;

  }

}


void show_occupied_cells(int CELL_TYPE){
  cout << "OCCUPIED CELLS [ ";
  for(int i=1;i<10;i++){
    int temp = 1 << i;
    if(CELL_TYPE & temp)
      cout << i << " ";
  }
  cout << "]"<< endl;
}

int colored_segmentation(const sensor_msgs::PointCloud2ConstPtr& cloud_data){
  int answer = 0;
  
        bool publish    = true;    
	bool show 	= true;      // Show Terminal ( x,y,z )
	bool show_color = false;       // Output topic
	bool show_coord = false;       // Show Terminal ( coordinates ) 
	pcl::search::Search <pcl::PointXYZRGB>::Ptr tree = boost::shared_ptr<pcl::search::Search<pcl::PointXYZRGB> > (new pcl::search::KdTree<pcl::PointXYZRGB>);

	// -------------------------------------------------------------------------------------------------------------
	// Filter & limit the image
	// -------------------------------------------------------------------------------------------------------------
	
	pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZRGB>);
	pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_filtered(new pcl::PointCloud<pcl::PointXYZRGB>);
	pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud_final(new pcl::PointCloud<pcl::PointXYZRGB>), cloud_p(new pcl::PointCloud<pcl::PointXYZRGB>),cloud_f(new pcl::PointCloud<pcl::PointXYZRGB>);

	pcl::fromROSMsg(*cloud_data,*cloud);

	// create the filtering object
	pcl::PassThrough<pcl::PointXYZRGB> pass, pass2;
	pass.setInputCloud(cloud);
	pass.setFilterFieldName("x");
	pass.setFilterLimits(-0.2,0.08);
	pass.filter(*cloud_filtered);
	
	pass2.setInputCloud(cloud_filtered);
	pass2.setFilterFieldName("y");
	pass2.setFilterLimits(-0.15,0.1);
	
	pass2.filter(*cloud_final);

	
	// -------------------------------------------------------------------------------------------------------------
	// Segmentation
	// -------------------------------------------------------------------------------------------------------------
	
	pcl::ModelCoefficients::Ptr coefficients (new pcl::ModelCoefficients);
	pcl::PointIndices::Ptr inliers(new pcl::PointIndices);
	
	// create segmentation object
	pcl::SACSegmentation<pcl::PointXYZRGB> seg;
	seg.setOptimizeCoefficients(true);
	seg.setModelType(pcl::SACMODEL_PLANE);
	seg.setMethodType(pcl::SAC_RANSAC);
	seg.setMaxIterations (1000);
	seg.setDistanceThreshold(0.01);
	
	seg.setInputCloud(cloud_final);
	seg.segment(*inliers,*coefficients);
	
	// create filtering object
	pcl::ExtractIndices<pcl::PointXYZRGB> extract;
	
	int i=0, nr_points = (int) cloud_final->points.size();
	
	// while 30% of cloud is still there
	while(cloud_final->points.size() > 0.1*nr_points){
	  // segment largest planar components from the remaining cloud
	  seg.setInputCloud(cloud_final);
	  seg.segment(*inliers,*coefficients);
	  if(inliers->indices.size() == 0){
	    cout << "could not estimate " <<endl;
	    break;
	  }
	  // extract inliers
	  extract.setInputCloud(cloud_final);
	  extract.setIndices(inliers);
	  extract.setNegative(false);
	  extract.filter(*cloud_p);
	  //cout << "Point cloud representing the planar component : " << cloud_p->width*cloud_p->height << " data points.\n";
	  extract.setNegative(true);
	  extract.filter(*cloud_f);
	  cloud_final.swap(cloud_f);
	  i++;
	  if(publish){
	    sensor_msgs::PointCloud2 output_cloud;
	    pcl::toROSMsg(*cloud_final,output_cloud);
	    pub.publish(output_cloud);
	    break;
	  }
	}
	
	
	// -------------------------------------------------------------------------------------------------------------
	// Extract Indices
	// -------------------------------------------------------------------------------------------------------------
	pcl::IndicesPtr indices(new std::vector<int>);
	pcl::PassThrough<pcl::PointXYZRGB> pass3;
	pass3.setInputCloud(cloud_final);
	pass3.setFilterFieldName("z");
	pass3.setFilterLimits(0.0,1.0);
	pass3.filter(*indices);
	
	pcl::RegionGrowingRGB<pcl::PointXYZRGB> reg;
	reg.setInputCloud(cloud_final);
	reg.setIndices(indices);
	reg.setSearchMethod(tree);
	reg.setDistanceThreshold(10);
	reg.setPointColorThreshold(6);
	reg.setRegionColorThreshold(5);
	reg.setMinClusterSize(600);
	
	pcl::PointCloud<pcl::PointXYZRGB>::Ptr colored_cloud = reg.getColoredCloud();
	
	
	
	// For publishing the image
	if(show_color){
	  sensor_msgs::PointCloud2 output_cloud;
	  pcl::toROSMsg(*cloud_final,output_cloud);
	  pub.publish(output_cloud);
	}
	
	std::vector<pcl::PointIndices> clusters;
	reg.extract(clusters);
	
	cout << "cluster size : " << clusters.size() << endl;
	coordinates coords[clusters.size()];
       

	if(show){
	  for(int i=0;i<clusters.size();i++){
	    
	    //improved
	    float min_x = 1000, max_x = -1000,
	      min_y = 1000, max_y = -1000,
	      min_z = 1000, max_z = -1000;
	    
	    for(int j=0,k=0;j<clusters[i].indices.size();j++,k++){
	      int index = clusters[i].indices[j];
	      
	      float x = cloud_final->points[index].x,
		y =cloud_final->points[index].y,
		z = cloud_final->points[index].z;
	      
	      if(x < min_x)
		min_x = x;
	      if(x > max_x)
		max_x = x;
	      if(y < min_y)
		min_y = y;
	      if(y > max_y)
		max_y = y;
	      if(z < min_z)
		min_z = z;
	      if(z > max_z)
		max_z = z;			
	    }
	    
	    double width = 0.031;
	    double decimal_limit = 0.001;
	    
	    coords[i].min_x = (int)(min_x/decimal_limit)*decimal_limit;
	    coords[i].max_x = ((int)(min_x/decimal_limit)*decimal_limit) + width;
	    coords[i].min_y = (int)(min_y/decimal_limit)*decimal_limit;
	    coords[i].max_y = ((int)(min_y/decimal_limit)*decimal_limit) + width;
	    
	   
	  }
	  for(int i=0;i<clusters.size();i++){
	    double x_range = (coords[i].min_x+coords[i].max_x)/2, y_range = (coords[i].min_y + coords[i].max_y)/2;
	    int r = get_row(y_range), c = get_col(x_range);
	    int cell = get_cell(r,c);
	    int temp = 1 << cell;
	    OCCUPIED_CELL |= temp;
	    
	    if(!CELL_OCCUPIED[cell]){
	      answer = cell;
	      CELL_OCCUPIED[cell] = true;
	    }
	    
	  }
	  show_occupied_cells(OCCUPIED_CELL);
	}
	return answer;
}




#endif 
