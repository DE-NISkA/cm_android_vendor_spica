#if ! defined( GET_VERT_FUNC ) || ! defined (IND_TYPE)
#error "Define GET_VERT_FUNC and IND_TYPE"
#endif
unsigned int GET_VERT_FUNC(int count , IND_TYPE *ind, int *pmin)
{
        int i = 0 ; 
        int max = 0 ; 
        int min = 0x7FFFFFFF; 
        for ( i = 0 ; i < count ; i++) 
        {
                if ( min > ind[i] )
                {
                        min = ind[i]; 
                }
                if ( max < ind[i] )
                {
                        max = ind[i];
                }
        }
        if ( pmin )
        {
                *pmin = min ; 
        }
        return (unsigned int )max ; 
}
#undef GET_VERT_FUNC
#undef IND_TYPE
