classdef Track_GNN < TrackerComponent
  %TRACK of GNN simple
  
  properties
    branchID = 0
    trackID = 0
    score = nan
    isConfirmed = false
    nLost = 0   
    windowSize = 8
    measIDHist
    numHist = 250
  end
  
  properties (SetAccess = ?TrackerComponent)    
    %> tracking filter (state prediction & estimation)
    filter
    %history
    hist
    histTime
  end
  
  properties (Dependent)
    tUpdate
    vec
    cov
  end
  
  
  methods  %Dependent
    
    function tUpdate = get.tUpdate(this)
      tUpdate = this.filter.tUpd;
    end
    
    function vec = get.vec(this)
      vec = this.filter.vec;
    end
    
    function cov = get.cov(this)
      cov = this.filter.cov;
    end
    
  end
  
  
  methods (Access = public)    
    
    function this = Track_GNN(varargin)
      if nargin
        this = Set(this, varargin{:});
      end
      
      if isempty(this.filter)
        %create default tracking filter
        this.filter = PolynomFilter;
      end
      
      %association history window size
      if isempty(this.windowSize)
        this.windowSize = 8;
      end
      
      %meas ID history
      this.measIDHist = CycleArray('sample', nan, 'capacity', this.windowSize);
      
      %estimated position history
      this.hist = CycleArray('capacity', this.numHist, 'sample', nan(6,1));
      
      %time history
      this.histTime = CycleArray('capacity', this.numHist, 'sample', nan);
    end
    
    
    function this = Init(this, meas)
      %Initiate new track
      this = Reset(this);
      
      %init tracking filter
      if nargin == 2
        this.filter = Init(this.filter, meas);
      elseif nargin == 4
        this.filter = Init(this.filter, meas, p2cVec, p2cMat);
      end
      
      %add to hist
      this = FillHistory(this);
    end
    
        
    function this = Predict(this, t)
      %predict tracking filter
      this.filter = Predict(this.filter, t);
    end
    
    
    function this = Update(this, meas)                        
      %update tracking filter
      this.filter = Update(this.filter, meas);
      
      %add to hist
      this = FillHistory(this);
    end
    
    
    function this = FillHistory(this)
      %add to hist
      this.hist = Add(this.hist, this.filter.vec(1:6));
      this.histTime = Add(this.histTime, this.tUpdate);
    end
    
    
    function this = Reset(this)
      this.trackID = 0;
      this.branchID = 0;
      this.filter = Reset(this.filter);
      this.hist = Reset(this.hist);
      this.histTime = Reset(this.histTime);
      this.nLost = 0;
      this.isConfirmed = false;
    end
    
  end
    
end