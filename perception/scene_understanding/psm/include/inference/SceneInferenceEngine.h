#pragma once

// Global includes
#include <queue>

// Package includes
#include <ros/ros.h>
#include <rosbag/view.h>

#include <pbd_msgs/PbdObject.h>
#include <pbd_msgs/PbdSceneGraph.h>

#include <visualization/gnuplot/GnuplotVisualization.h>
#include <visualization/psm/ProbabilisticSceneModelVisualization.h>

// Local includes
#include "helper/ObjectTransformation.h"

#include "inference/model/SceneIdentifier.h"
#include "inference/model/SceneModelDescription.h"

namespace ProbabilisticSceneRecognition {
  
  /**
   * Probabilistic scene inference engine in form of a ROS node. This class is basically a ROS wrapper for the inference model. Engine parameters are loaded via node handle, the model is loaded from file.
   *
   * @author Joachim Gehrung
   * @version See SVN
   */
  class SceneInferenceEngine {
  public:

    /**
     * Constructor.
     */
    SceneInferenceEngine();

    /**
     * Destructor.
     */
    ~SceneInferenceEngine();
    
    /**
     * Updates the inference engine.
     */
    void update();
    
    /**
     * Runs a single update and then terminates.
     */
    void executeInStackMode();
    
  private:
    
    /**
     * Loads the probabilistic scene model from XMl file.
     *
     * @param pSceneModelFileName The name of the XMl file containing the scene model that should we used.
     * @param pInferenceAlgorithm The name of the inference algorithm.
     */
    void loadSceneModel(const std::string pSceneModelFileName, const std::string pInferenceAlgorithm);
    
    /**
     * Extract PbdSceneGraph messages from all rosbag files given as CLI parameters.
     * 
     * @param pInputBagFilenames A list of the bag files that contain the learning data.
     */
    void readLearnerInputBags(XmlRpc::XmlRpcValue pInputBagFilenames);
    
    /**
     * Open rosbag file and extract PbdSceneGraph messages on input topic (which has been set before).
     * 
     * @param pPbdSceneGraphsBagPath Path of file to be parsed for PbdSceneGraph messages.
     */
    void extractPbdSceneGraphsFromBag(const std::string& pPbdSceneGraphsBagPath);
    
    /**
     * Initializes the chain responsible for visualization.
     * 
     * @param pScale Factor to multiply the kernel with.
     * @param pSigmaMultiplicator Scaling factor for the size of the visualized covariance ellipsoid.
     * @param pFrameId The name of the coordinate frame that should be drawn into.
     */
    void initializeVisualizationChain(const double pScale, const float pSigmaMultiplicator, const std::string pFrameId);
    
    /**
     * Collects evidences in form of PbdObject and forwards them to the inference model.
     *
     * @param pObject An observation result for a potential scene element coming from an arbitrary sensor data processing system.
     */
    void newObservationCallback(const boost::shared_ptr<pbd_msgs::PbdObject>& pObject);

    /**
     * Collects scene examples in form of PbdSceneGraph messages and forwards them to the visualization.
     *
     * @param pSceneGraph Preprocessed observations that describe the objects in a scene over time.
     */
    void newSceneGraphCallback(const boost::shared_ptr<const pbd_msgs::PbdSceneGraph>& pSceneGraph);
    
  private:
    
    /**
     * True to show the plot of the scene probabilities.
     */
    bool showPlot;
    
    /**
     * Set true to overwrite the visualization of results and plot the target distributions instead.
     */
    bool mTargetingHelp;
    
    /**
     * Interface to private ros node namespace.
     */
    ros::NodeHandle mNodeHandle;
    
    /**
     * A callback handler listening to objects found by an object detection system.
     */
    ros::Subscriber mObjectListener;
    
    /**
     * A callback handler listening to preprocessed observations that describe the objects in a scene over time
     */
    ros::Subscriber mSceneGraphListener;
    
    /**
     * A buffer for storing evidences.
     */
    std::queue<boost::shared_ptr<pbd_msgs::PbdObject> > mEvidenceBuffer;
    
    /**
     * A buffer for storing scene graphs.
     */
    std::queue<boost::shared_ptr<const pbd_msgs::PbdSceneGraph> > mSceneGraphBuffer;
    
    /**
     * A transformer for objects into the target coordinate frame.
     */
    ObjectTransformation mObjectTransform;
    
    /**
     * The model is responsible for loading the scene model from file, collect and manage the evidence and do the inference.
     */
    SceneModelDescription mModel;
    
    /**
     * Gnuplot visualizer for drawing bar diagrams.
     */
    Visualization::GnuplotVisualization mVisGnuplot;
    
    /**
     * Class for coordinating the scene visualizers.
     */
    boost::shared_ptr<Visualization::ProbabilisticSceneModelVisualization> mVisualizer;
  };
}