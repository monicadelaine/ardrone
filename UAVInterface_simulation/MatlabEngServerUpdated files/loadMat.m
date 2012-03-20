if start==0  
    load ucells
    load occlusions
    load occlusion_status
    load ROI_orig
    javaaddpath(pwd);
    f2=imread('croi_new.jpg');

    % Cell data
    if ~isempty(ROI)
        ourcells=ucells(ROI,:);
        our_occlusion_status=occlusion_status(ROI,:);
        our_occlusions=occlusions(ROI,:);
        numCells = size(ourcells,1);
    else
        ourcells=ucells;
        our_occlusion_status=occlusion_status;
        our_occlusions=occlusions;
        numCells = size(ucells,1);
    end
    
    % Target data
    %start_targets=uint32(rand(numTargets,1)*numCells);
    load start_targets10
    numTargets = size(start_targets,1);  
    
    %    maxUnseen          - max timesteps each viewable cell can go unseen
    %    steps              - number of timesteps to simulate
    %    alulval            - unknown?????
    %    dynamic_t          - flag for dynamic targets
    %    alulFlag           - flag to use ALUL
    
    maxUnseen = 150;
    steps = 100;
    alulval = 0;
    dynamic_t = 0;
    %alulFlag = 0; 
        
    ALUL_THRESHOLD = 15;
    
    t = 2;
    start = 1;
    sim_start = 0;
    
    dov = 19;
    %dov = 20;
    cameras=[5131 dov; 5136 dov];
    %cameras=[5203 dov; 5241 dov; 2283 dov; 2321 dov];
end
