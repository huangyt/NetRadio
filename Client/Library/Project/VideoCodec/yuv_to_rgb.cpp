#include "Stdafx.h"
#include "TypeDefine.h"
#include "ColorSpaceAPI.h"

#define MAXIMUM_Y_WIDTH 1024

//=============================================================================
static uint64_t mmw_mult_Y    = 0x2568256825682568;
static uint64_t mmw_mult_U_G  = 0xf36ef36ef36ef36e;
static uint64_t mmw_mult_U_B  = 0x40cf40cf40cf40cf;
static uint64_t mmw_mult_V_R  = 0x3343334333433343;
static uint64_t mmw_mult_V_G  = 0xe5e2e5e2e5e2e5e2;


static uint64_t mmb_0x10      = 0x1010101010101010;
static uint64_t mmw_0x0080    = 0x0080008000800080;
static uint64_t mmw_0x00ff    = 0x00ff00ff00ff00ff;

static uint64_t mmw_cut_red   = 0x7c007c007c007c00;
static uint64_t mmw_cut_green = 0x03e003e003e003e0;
static uint64_t mmw_cut_blue  = 0x001f001f001f001f;

//=============================================================================
void yuv420_to_rgb(int x_dim,  int y_dim, uint8_t *puc_out, int stride_out, 
	uint8_t* puc_y,  int stride_y, uint8_t *puc_u, uint8_t *puc_v, int stride_uv)
{
	int y, horiz_count;
	uint8_t *puc_out_remembered;
	//int stride_out = x_dim * 3;

	if (y_dim < 0) {
		//we are flipping our output upside-down
		y_dim  = -y_dim;
		puc_y     += (y_dim   - 1) * stride_y ;
		puc_u     += (y_dim/2 - 1) * stride_uv;
		puc_v     += (y_dim/2 - 1) * stride_uv;
		stride_y  = -stride_y;
		stride_uv = -stride_uv;
	}

	horiz_count = -(x_dim >> 3);

	for (y=0; y<y_dim; y++) {
		if (y == y_dim-1) {
			//this is the last output line - we need to be careful not to overrun the end of this line
			uint8_t temp_buff[3*MAXIMUM_Y_WIDTH+1];
			puc_out_remembered = puc_out;
			puc_out = temp_buff; //write the RGB to a temporary store
		}
		_asm {
			push eax
				push ebx
				push ecx
				push edx
				push edi

				mov eax, puc_out       
				mov ebx, puc_y       
				mov ecx, puc_u       
				mov edx, puc_v
				mov edi, horiz_count

horiz_loop:

			movd mm2, [ecx]
			pxor mm7, mm7

				movd mm3, [edx]
				punpcklbw mm2, mm7       

					movq mm0, [ebx]          
					punpcklbw mm3, mm7       

						movq mm1, mmw_0x00ff     

						psubusb mm0, mmb_0x10    

						psubw mm2, mmw_0x0080    
						pand mm1, mm0            

						psubw mm3, mmw_0x0080    
						psllw mm1, 3             

						psrlw mm0, 8             
						psllw mm2, 3             

						pmulhw mm1, mmw_mult_Y   
						psllw mm0, 3             

						psllw mm3, 3             
						movq mm5, mm3            

						pmulhw mm5, mmw_mult_V_R 
						movq mm4, mm2            

						pmulhw mm0, mmw_mult_Y   
						movq mm7, mm1            

						pmulhw mm2, mmw_mult_U_G 
						paddsw mm7, mm5

						pmulhw mm3, mmw_mult_V_G
						packuswb mm7, mm7

						pmulhw mm4, mmw_mult_U_B
						paddsw mm5, mm0      

						packuswb mm5, mm5
						paddsw mm2, mm3          

						movq mm3, mm1            
						movq mm6, mm1            

						paddsw mm3, mm4
						paddsw mm6, mm2

						punpcklbw mm7, mm5
						paddsw mm2, mm0

						packuswb mm6, mm6
						packuswb mm2, mm2

						packuswb mm3, mm3
						paddsw mm4, mm0

						packuswb mm4, mm4
						punpcklbw mm6, mm2

						punpcklbw mm3, mm4

						// 32-bit shuffle.
						pxor mm0, mm0

						movq mm1, mm6
						punpcklbw mm1, mm0

						movq mm0, mm3
						punpcklbw mm0, mm7

						movq mm2, mm0

						punpcklbw mm0, mm1
						punpckhbw mm2, mm1

						// 24-bit shuffle and sav
						movd   [eax], mm0
						psrlq mm0, 32

						movd  3[eax], mm0

						movd  6[eax], mm2


						psrlq mm2, 32            

						movd  9[eax], mm2        

						// 32-bit shuffle.
						pxor mm0, mm0            

						movq mm1, mm6            
						punpckhbw mm1, mm0       

						movq mm0, mm3            
						punpckhbw mm0, mm7       

						movq mm2, mm0            

						punpcklbw mm0, mm1       
						punpckhbw mm2, mm1       

						// 24-bit shuffle and sav
						movd 12[eax], mm0        
						psrlq mm0, 32            

						movd 15[eax], mm0        
						add ebx, 8               

						movd 18[eax], mm2        
						psrlq mm2, 32            

						add ecx, 4               
						add edx, 4               

						movd 21[eax], mm2        
						add eax, 24              

						inc edi
						jne horiz_loop

						pop edi
						pop edx
						pop ecx
						pop ebx
						pop eax

						emms
		}


		if (y == y_dim-1) {
			//last line of output - we have used the temp_buff and need to copy
			int x = 3 * x_dim;                  //interation counter
			uint8_t *ps = puc_out;                // source pointer (temporary line store)
			uint8_t *pd = puc_out_remembered;     // dest pointer
			while (x--) *(pd++) = *(ps++);          // copy the line
		}

		puc_y   += stride_y;
		if (y%2) {
			puc_u   += stride_uv;
			puc_v   += stride_uv;
		}
		puc_out += stride_out; 
	}
}
