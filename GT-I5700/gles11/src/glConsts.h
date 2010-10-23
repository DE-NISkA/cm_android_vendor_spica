#ifndef __glconsts_h__
#define ___glconsts_h__

#ifdef __cplusplus
extern "C" {
#endif

/* String State */
#define C_VENDOR                                                "SAMSUNG ELECTRONICS"

/* Implementation Dependent Values */
#define C_MAX_CLIP_PLANES                                6
#define C_MAX_MODELVIEW_STACK_DEPTH      64 
#define C_MAX_PROJECTION_STACK_DEPTH     64
#define C_MAX_TEXTURE_STACK_DEPTH        64
#define C_SUBPIXEL_BITS                  16
#define C_MAX_TEXTURE_SIZE                               2048
#define C_MAX_VIEWPORT_DIMS                              {320,240}

#define C_ALIASED_POINT_SIZE_MIN                1        
#define C_ALIASED_POINT_SIZE_MAX                6
#define C_POINT_FADE_THRESHOLD 16
#define C_POINT_DISTANCE_ATTENUATION    1

#define C_ALIASED_POINT_SIZE_RANGE      
#define C_SMOOTH_POINT_SIZE_RANGE                       
#define C_SMOOTH_POINT_SIZE_GRANULARITY         
#define C_ALIASED_LINE_WIDTH_RANGE              
#define C_SMOOTH_LINE_WIDTH_RANGE                       
#define C_SMOOTH_LINE_WIDTH_GRANULARITY         

#define C_MAX_ELEMENTS_INDICES          
#define C_MAX_ELEMENTS_VERTICES         
#define C_MAX_TEXTURE_UNITS                             2
#define C_SAMPLE_BUFFERS                                0                       
#define C_SAMPLES                                               0
#define C_COMPRESSED_TEXTURE_FORMATS texPalFormats



#define C_NUM_COMPRESSED_TEXTURE_FORMATS  10    
#define C_MAX_ERRORS                                    10

#define C_MAX_VERTEX_UNITS_OES                  4
#define C_MAX_PALETTE_MATRICES_OES              10
#define C_PI 3.14159265358979323846
#define C_MAX_LIGHTS                    8 

/* Enabled Flags */
#define SETBIT(x)                                               (1<<(x))                                        
#define F_MATRIX_PALETTE_OES                    SETBIT(0)
#define F_FOG                                                   SETBIT(1)
#define F_LIGHTING                      SETBIT(2)
#define F_TEXTURE_2D_UNIT0              SETBIT(3)
#define F_TEXTURE_2D_UNIT1              SETBIT(4)
#define F_CULL_FACE                     SETBIT(5)
#define F_ALPHA_TEST                    SETBIT(6)
#define F_BLEND                         SETBIT(7)
#define F_COLOR_LOGIC_OP                SETBIT(8)
#define F_DITHER                        SETBIT(9)
#define F_STENCIL_TEST                  SETBIT(10)
#define F_DEPTH_TEST                    SETBIT(11)
#define F_POINT_SMOOTH                  SETBIT(12)
#define F_LINE_SMOOTH                   SETBIT(13)
#define F_SCISSOR_TEST                  SETBIT(14)
#define F_COLOR_MATERIAL                SETBIT(15)
#define F_NORMALIZE                     SETBIT(16)
#define F_RESCALE_NORMAL                SETBIT(17)
#define F_POLYGON_OFFSET_FILL           SETBIT(18)
#define F_VERTEX_ARRAY                  SETBIT(19)
#define F_NORMAL_ARRAY                  SETBIT(20)
#define F_COLOR_ARRAY                   SETBIT(21)
#define F_TEXTURE_COORD_ARRAY0          SETBIT(22)
#define F_TEXTURE_COORD_ARRAY1          SETBIT(23)
#define F_MULTISAMPLE                   SETBIT(24)
#define F_SAMPLE_ALPHA_TO_COVERAGE      SETBIT(25)
#define F_SAMPLE_ALPHA_TO_ONE           SETBIT(26)
#define F_SAMPLE_COVERAGE               SETBIT(27)
#define F_MATRIX_INDEX_ARRAY_OES        SETBIT(28)
#define F_WEIGHT_ARRAY_OES              SETBIT(29)

#define F_POINT_SPRITE_OES              SETBIT(0)
#define F_POINT_SIZE_ARRAY_OES           SETBIT(1)
#define F_FOG_USE                                        SETBIT(2)
#ifdef __cplusplus
}       
#endif

#endif /* __glconsts_h__ */
