/*
*-------------------------------------------------------------------
* swdMat.cpp
* Copyright(c) 2019, The RgbVideoEditor Author (Shunguang Wu).
* All Rights Reserved.
* You may not use this file except in compliance with the License:
*      GNU GENERAL PUBLIC LICENSE (Version 3), 29 June 2007
*
* You may obtain a copy of the License at
*
*     https://www.gnu.org/licenses/gpl-3.0.en.html
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*-------------------------------------------------------------------
*/

#include "swdMat.h"
#include <conio.h>
using namespace std;

swdMat::swdMat( const unsigned int r, const unsigned int c ):
m_buf( NULL ),
m_rows( r ),
m_cols( c ),
m_size ( r * c )
{
if ( m_size > 0 )
creatBuf();
}


swdMat::swdMat(const unsigned int r, const unsigned int c, const double initValue):
m_buf( NULL ),
m_rows( r ),
m_cols( c ),
m_size ( r * c )
{
creatBuf();

for (unsigned int k=0; k<m_size; k++)
m_buf[k] = initValue;
}

// constructor from a buffer
swdMat::swdMat( const unsigned int r, const unsigned int c, const double *buf, const unsigned int bufLength ):
m_buf( NULL ),
m_rows( r ),
m_cols( c ),
m_size ( r * c )
{
// NOTE: since we do not know the length of buf, there is a pentential risk to use this function
if (  bufLength != m_size ){
errMsg( "Warning! swdMat::swdMat(): the input size not match the buffer size." );
}

creatBuf();

for ( unsigned int k=0; k<m_size; k++ )
m_buf[k] = buf[k];
}

// constructor from a matrix
swdMat::swdMat( const swdMat &x ):
m_buf( NULL ),
m_rows( x.rows() ),
m_cols( x.cols() ),
m_size( x.size() )
{
creatBuf();

for( unsigned int k=0; k<m_size; k++ )
m_buf[k] = x.m_buf[k];
}

swdMat::swdMat(const unsigned int nCols, const char* fileName):
m_buf( NULL ),
m_rows( 1 ),
m_cols( nCols ),
m_size( nCols )
{
creatBuf();
readFromFile( nCols, fileName );
}

swdMat::~swdMat() 
{ 
deleteBuf(); 
}

unsigned int swdMat :: cols() const 
{ 
return m_cols; 
}

const double* swdMat :: data() const 
{
return m_buf; 
}               

double* swdMat :: getBuf()
{
return m_buf; 
}

unsigned int swdMat :: rows() const 
{ 
return m_rows; 
}

//asign the data of matrix to a double point
void swdMat :: data( double **pp )
{
for( unsigned int i=0; i<m_rows; i++) 
*(pp+i) = &m_buf[ i * m_cols];
}

unsigned int swdMat::size() const 
{ 
return m_size; 
}

void swdMat :: print(const char* fmt, const char* str) const
{
if( m_rows == 1 )
printf("%s (%d, %d)= [", str, m_rows, m_cols);
else
printf("%s (%d, %d)= \n[", str, m_rows, m_cols);

for ( unsigned int i=0; i<m_rows; i++ ){

if (i>0){
printf(" ");
}

for( unsigned int j=0; j<m_cols; j++ )
printf(fmt, m_buf[i*m_cols + j]);
if ( i < m_rows-1 )
printf(";\n");
}
printf("];\n"); 
}

void swdMat :: print(const char* str) const
{
print("%f ", str);
}


void swdMat :: cprint(const char* str) const
{
if( m_rows == 1 )
_cprintf("%s (%d, %d)= [", str, m_rows, m_cols);
else
_cprintf("%s (%d, %d)= \n [", str, m_rows, m_cols);

for ( unsigned int i=0; i<m_rows; i++ ){
for( unsigned int j=0; j<m_cols; j++ )
_cprintf("%f", m_buf[i*m_cols + j]);
if ( i < m_rows-1 )
_cprintf(";\n");
}
_cprintf("];\n"); 
}

//change the size, donot keep the previous value
void swdMat::resize( const unsigned int newRows, const unsigned int newCols )
{
if ( m_size != newRows * newCols ) { //reallocate memory, uninitinalized 
deleteBuf();
m_size = newRows * newCols;
m_rows = newRows;
m_cols = newCols;
creatBuf();
}
else { //leave the original value there, 
m_size = newRows * newCols;
m_rows = newRows;
m_cols = newCols;
}
}
void swdMat::resize( const unsigned int newRows, const unsigned int newCols, const double initVal )
{
resize( newRows, newCols );
for (unsigned int k=0; k<m_size; k++)
m_buf[k] = initVal;
}

//keep the values, just change the shape
void swdMat::reshape( const unsigned int newRows, const unsigned int newCols )
{
#if SW_MAT_BOUNDS_CHECK
if ( m_size != newRows * newCols ) { 
errMsg( "Warning: swdMat::reshape() -- input parameters are not match the orioginal ones!" );
}
#endif

// leave the original value there, now they are stored still in the order of original (raw by raw)
m_rows = newRows;
m_cols = newCols;
}


//-----------------------------------------
// public operator overloads
//-----------------------------------------
//overload assignment operator
//const return avoid: ( a1=a2 ) = a3
swdMat &swdMat::operator =(const swdMat &x) //assignment
{
if (&x != this ){ //check for self assignment
if ( m_size != x.size() ){
deleteBuf();
m_size = x.size();
creatBuf();
}

m_rows = x.rows();
m_cols = x.cols();
for( unsigned int k=0; k<m_size; k++ )
m_buf[k] = x.m_buf[k];
}

return *this; // enables x=y=z;
}

swdMat &swdMat::operator =(const double d) //assignment
{
for( unsigned int k=0; k<m_size; k++ )
m_buf[k] = d;

return *this;
}

bool swdMat::operator==( const swdMat &x) const
{
if ( x.size() != m_size )
return false;

for( unsigned int k=0; k<m_size; k++)
if (m_buf[k] != x.m_buf[k])
return false;

return true;
}

bool swdMat::operator!=( const swdMat &x) const
{
return ( ! ( *this == x ) );
}

swdMat swdMat::operator-() const
{
swdMat tmp(m_rows, m_cols);
for( unsigned int k=0; k<m_size; k++ )
tmp.m_buf[k] = -m_buf[k];

return tmp;
}

//a+x  add by element
swdMat swdMat::operator +(const swdMat &x) const
{
#if SW_MAT_BOUNDS_CHECK
if ( this->m_rows != x.rows() || this->m_cols != x.cols() )
errMsg( "swdMat::operator +(const swdMat &x): size is not match!" );
#endif

swdMat tmp = *this;
for ( unsigned int k=0; k<this->m_size; k++ )
tmp.m_buf[k] += x.m_buf[k];

return tmp;
}

//a+scale  add by element
swdMat swdMat::operator +(const double scale) const
{
swdMat tmp = *this;
for ( unsigned int k=0; k<m_size; k++ )
tmp.m_buf[k] += scale;

return tmp;
}

//a-x  minus by element
swdMat swdMat::operator -(const swdMat &x) const
{
#if SW_MAT_BOUNDS_CHECK
if ( this->m_rows != x.rows() || this->m_cols != x.cols() )
errMsg( "swdMat::operator -(const swdMat &x): size is not match!" );
#endif

swdMat tmp = *this;
for ( unsigned int k = 0; k < this->m_size; k++ )
tmp.m_buf[k] -= x.m_buf[k];

return tmp;
}

//a-scale  minus by scale
swdMat swdMat::operator -(const double scale) const
{
swdMat tmp = *this;
for ( unsigned int k=0; k<this->m_size; k++ )
tmp.m_buf[k] -= scale;

return tmp;
}

//a*x  time by element
swdMat swdMat::operator *(const swdMat &x) const
{
#if SW_MAT_BOUNDS_CHECK
if ( this->m_rows != x.rows() || this->m_cols != x.cols() )
errMsg( "swdMat::operator *(const swdMat &x): size is not match!" );
#endif

swdMat tmp = *this;
for ( unsigned int k=0; k<this->m_size; k++ )
tmp.m_buf[k] *= x.m_buf[k];

return tmp;
}

//a*scale  add by element
swdMat swdMat::operator *(const double scale) const
{
swdMat tmp = *this;
for ( unsigned int k=0; k<this->m_size; k++ )
tmp.m_buf[k] *= scale;

return tmp;
}

//a/x  add by element
swdMat swdMat::operator /(const swdMat &x) const
{
#if SW_MAT_BOUNDS_CHECK
if ( this->m_rows != x.rows() || this->m_cols != x.cols() )
errMsg( "swdMat::operator /(const swdMat &x): size is not match!" );
#endif

swdMat tmp = *this;
for ( unsigned int k=0; k<this->m_size; k++ )
tmp.m_buf[k] /= x.m_buf[k];

return tmp;
}

//a/scale  add by element
swdMat swdMat::operator /(const double scale) const
{
swdMat tmp = *this;
for ( unsigned int k=0; k<this->m_size; k++ )
tmp.m_buf[k] /= scale;

return tmp;
}

// matrix production  A % X
swdMat swdMat::operator %(const swdMat& x) const
{
#if SW_MAT_BOUNDS_CHECK
if ( this->cols() != x.rows() )
errMsg( "swdMat::operator %(const swdMat &x): size is not match!" );
#endif

unsigned int m = this->rows();
unsigned int n = this->cols();
unsigned int k = x.cols();

swdMat tmp( m, k, 0.0);
for ( unsigned int i=0; i<m; i++)
{
for ( unsigned int j=0; j<k; j++)
{
for ( unsigned int l=0; l<n; l++) 
tmp.m_buf[ i*k + j ] += ( this->m_buf[i*n + l] * x.m_buf[l*k + j] );
}
}

return tmp;
}

double swdMat :: operator()(const unsigned int i, const unsigned int j) const
{
#if SW_MAT_BOUNDS_CHECK
if ( i<0 || i>=this->m_rows || j<0 || j>=this->m_cols ){
errMsg( "swdMat::operator(): subscript out of bounds!" );
}
#endif

return this->m_buf[i*m_cols + j];
}


double& swdMat :: operator()(const unsigned int i, const unsigned int j)
{
#if SW_MAT_BOUNDS_CHECK
if ( i<0 || i>=this->m_rows || j<0 || j>=this->m_cols ){
errMsg( "swdMat::operator(): subscript out of bounds!" );
}
#endif

return this->m_buf[i*m_cols + j];
}

double swdMat :: operator()(const unsigned int k) const
{
#if SW_MAT_BOUNDS_CHECK
if ( k<0 || k>=(this->m_rows * this-> m_cols) ){
errMsg( "swdMat::operator(): subscript out of bounds!" );
}
#endif

return this->m_buf[k];
}

double &swdMat :: operator()(const unsigned int k)
{
#if SW_MAT_BOUNDS_CHECK
if ( k<0 || k>=(this->m_rows * this-> m_cols) ){
errMsg( "swdMat::operator(): subscript out of bounds!" );
}
#endif

return this->m_buf[k];
}

swdMat& swdMat :: operator +=( const swdMat &x)
{
#if SW_MAT_BOUNDS_CHECK
if ( this->m_rows != x.m_rows || this->m_cols != x.m_cols )
errMsg("Error! swdMat :: operator +=() size is not the same.");
#endif

for(unsigned int k=0; k<this->m_size; k++)
this->m_buf[k] += x.m_buf[k];

return *this;
}

swdMat& swdMat :: operator +=( const double scale)
{
for(unsigned int k=0; k<m_size; k++)
m_buf[k] += scale;

return *this;
}

swdMat& swdMat :: operator -=( const swdMat &x)
{
#if SW_MAT_BOUNDS_CHECK
if ( m_rows != x.m_rows || m_cols != x.m_cols )
errMsg("Error! swdMat :: operator -=() size is not the same.");
#endif

for(unsigned int k=0; k<m_size; k++)
m_buf[k] -= x.m_buf[k];

return *this;
}

swdMat& swdMat :: operator -=( const double scale)
{
for(unsigned int k=0; k<m_size; k++)
m_buf[k] -= scale;

return *this;
}

swdMat& swdMat :: operator *=( const swdMat &x)
{
#if SW_MAT_BOUNDS_CHECK
if ( m_rows != x.m_rows || m_cols != x.m_cols )
errMsg("Error! swdMat :: operator -=() size is not the same.");
#endif

for(unsigned int k=0; k<m_size; k++)
m_buf[k] *= x.m_buf[k];

return *this;
}

swdMat& swdMat :: operator *=( const double scale)
{
for(unsigned int k=0; k<m_size; k++)
m_buf[k] *= scale;

return *this;
}

swdMat& swdMat :: operator /=( const swdMat &x)
{
#if SW_MAT_BOUNDS_CHECK
if ( m_rows != x.m_rows || m_cols != x.m_cols )
errMsg("Error! swdMat :: operator -=() size is not the same.");
#endif

for(unsigned int k=0; k<m_size; k++)
m_buf[k] = m_buf[k] / x.m_buf[k];

return *this;
}

swdMat& swdMat :: operator /=( const double scale)
{
for(unsigned int k=0; k<m_size; k++)
m_buf[k] = m_buf[k] / scale;

return *this;
}

//--------------------------------------
// now the friend functions
//--------------------------------------
ostream& operator<<( ostream &os, swdMat &x )
{
x.print();
return os;
}

// 5+x
swdMat operator+( const double scale, const swdMat &x )
{
return (x + scale) ;
}

// 5-x
swdMat   operator-( const double scale, const swdMat &x )
{
return (-x + scale) ;
}

// 5*x
swdMat   operator*( const double scale, const swdMat &x ) 
{
return (x*scale) ;
}

// 5/x
swdMat   operator/( const double scale, const swdMat &x ) 
{
unsigned int m = x.rows();
unsigned int n = x.cols();

swdMat y(m,n);
for ( unsigned int i=0; i<m; i++ )
for (unsigned int j=0; j<n; j++)
y(i,j) = scale/x(i,j);

return y ;
}

double swdMat :: trace() const
{
double tr = 0;
for (unsigned int i=0; i<m_rows; i++)
tr += m_buf[i*m_cols + i ];

return tr;
}

void swdMat ::  setCol( const unsigned int c, const double val )
{
#if SW_MAT_BOUNDS_CHECK
if ( c<0 || c>= m_cols )
errMsg( "swdMat ::  setCol(): Column number out of order!" );
#endif

for (unsigned int i=0; i<m_rows; i++)
m_buf[i*m_cols + c ] = val;
}

// set the col elements as vals stored in buf[] 
void swdMat :: setCol( const unsigned int c, const double* buf, const unsigned int bufSize )
{
#if SW_MAT_BOUNDS_CHECK
if ( c<0 || c>= m_cols )
errMsg( "swdMat ::  setCol(): Column number out of order!" );

if ( bufSize <  m_rows )
errMsg( "swdMat ::  setCol(): not enough data in buf[]!" );
#endif

for (unsigned int i=0; i<m_rows; i++)
m_buf[i*m_cols + c ] = buf[i];
}

void swdMat ::  setCol( const unsigned int c, const swdMat &x )
{
#if SW_MAT_BOUNDS_CHECK
if ( c<0 || c>= m_cols )
errMsg( "swdMat ::  setCol(): Column number out of order!" );
#endif

for (unsigned int i=0; i<m_rows; i++)
m_buf[i*m_cols + c ] = x.m_buf[i];
}

void  swdMat :: setSubCol( const unsigned int c, const unsigned int r0, const unsigned int r1, const double val)
{
#if SW_MAT_BOUNDS_CHECK
if ( c<0 || c>= m_cols )
errMsg( "swdMat ::  setSubCol(): Column number out of order!" );

if ( r0<0 || r1>= m_rows )
errMsg( "swdMat ::  setSubCol(): Row range out of order!" );
#endif

for (unsigned int i=r0; i<=r1; i++)
m_buf[i*m_cols + c ] = val;

}

void  swdMat :: setSubCol( const unsigned int c, const unsigned int r0, const unsigned int r1, const swdMat &x)
{
#if SW_MAT_BOUNDS_CHECK
if ( c<0 || c >= m_cols )
errMsg( "swdMat ::  setSubCol(): Column number out of order!" );

if ( r0<0 || r1 >= m_rows )
errMsg( "swdMat ::  setSubCol(): Row range out of order!" );

if ( x.size() < r1-r0+1 )
errMsg( "swdMat ::  setSubCol(): not enough data in x!" );
#endif

unsigned int k=0;
for (unsigned int i=r0; i<=r1; i++){
m_buf[i*m_cols + c] = x.m_buf[k];
k++;
}
}

void swdMat :: setData( const double *buf, const unsigned int bufSize )
{
unsigned int n = (bufSize > m_size) ? m_size : bufSize ;

for (unsigned int i=0; i<n; i++)
m_buf[i] = buf[i];
}

void swdMat :: setDiagonal( const double d)
{
for (unsigned int i=0; i<m_rows; i++)
m_buf[i*m_cols + i ] = d;
}

void swdMat :: setDiagonal( const swdMat &d)
{
#if SW_MAT_BOUNDS_CHECK
if ( d.m_rows > 1 && d.m_cols > 1)
errMsg( "swdMat :: setDiaganol() d is not a vector!");
#endif

for (unsigned int i=0; i<m_rows; i++)
m_buf[i*m_cols + i ] = d.m_buf[i];
}

void swdMat :: setDiagonal( const double *buf, const unsigned int nBufSize )
{
#if SW_MAT_BOUNDS_CHECK
if ( m_rows > nBufSize )
errMsg( "swdMat :: setDiaganol() buffer size is too small!");
#endif

for (unsigned int i=0; i<m_rows; i++)
m_buf[i*m_cols + i ] = buf[i];
}

void swdMat :: setRow( const unsigned int &r, const double &val )
{
#if SW_MAT_BOUNDS_CHECK
if ( r<0 || r>= m_rows )
errMsg( "swdMat ::  setRow(): row number out of order!" );
#endif

for (unsigned int i=0; i<m_cols; i++)
m_buf[r*m_cols + i ] = val;

}

void  swdMat :: setRow( const unsigned int &r, const double *pSrc )
{
double *pDes = m_buf + r*m_cols;
memcpy( pDes, pSrc, m_cols*sizeof( double ) );
}

void swdMat :: setRow( const unsigned int &r, const swdMat &x, const bool &isByRow )
{
#if SW_MAT_BOUNDS_CHECK
if ( r<0 || r>= m_rows )
errMsg( "swdMat ::  setRow(): row number out of order!" );

if ( x.size() < m_cols )
errMsg( "swdMat ::  setRow(): the size of x is too small!" );
#endif
double *p = &m_buf[r*m_cols];
if( isByRow ){
const double *px = x.data();
for (unsigned int i=0; i<m_cols; i++)
*p++ = *px++;
}
else{ //by col.
for (unsigned int i=0; i<x.cols(); ++i){
for(unsigned int j=0; j<x.rows(); ++j){
*p++ = x(j,i);
}
}
}

}

// set the rows as vals stored in vector x 
void  swdMat :: setRows( const unsigned int r0, const unsigned int r1, const swdMat &x )
{
#if SW_MAT_BOUNDS_CHECK
if ( r0<0 || r1>= m_rows )
errMsg( "swdMat ::  setRows(): row number out of order!" );

if ( x.size() < (r1-r0+1)*m_cols )
errMsg( "swdMat ::  setRows(): the size of x is too small!" );
#endif

unsigned int k=0;
for (unsigned int i=r0; i<=r1; i++){
for (unsigned int j=0; j<m_cols; j++){
m_buf[i*m_cols + j ] = x.m_buf[k];
k++;
}
}
}

void  swdMat :: setSubRow( const unsigned int r, const unsigned int c0, const unsigned int c1, const double val)
{
#if SW_MAT_BOUNDS_CHECK
if ( r<0 || r >= m_rows )
errMsg( "swdMat ::  setSubRow(): row number out of order!" );

if ( c0<0 || c1 >= m_cols )
errMsg( "swdMat ::  setSubRow(): col number range is out of order!" );
#endif

for (unsigned int i=c0; i<=c1; i++)
m_buf[r*m_cols + i ] = val;
}

void  swdMat :: setSubRow( const unsigned int r, const unsigned int c0, const unsigned int c1, const swdMat &x)
{
#if SW_MAT_BOUNDS_CHECK
if ( r<0 || r >= m_rows )
errMsg( "swdMat ::  setSubRow(): row number out of order!" );

if ( c0<0 || c1 >= m_cols )
errMsg( "swdMat ::  setSubRow(): col number range is out of order!" );

if ( x.size() < c1-c0+1 )
errMsg( "swdMat ::  setSubRow(): not enough data in x!" );
#endif

unsigned int k=0;
for (unsigned int i=c0; i<=c1; i++){
m_buf[r*m_cols + i ] = x.m_buf[k];
k++;
}
}

void swdMat :: setSlice( const unsigned int r0, const unsigned int c0, //top left position
const unsigned int rowL, const unsigned int colL,        // row and col length
const double val )
{
#if SW_MAT_BOUNDS_CHECK
if ( r0 < 0 || c0 < 0 )
errMsg("swdMat :: setSlice() , left top position is out of range!");

if ( rowL <= 0 || colL <= 0 )
errMsg("swdMat :: setSlice() , the height and width is out of range!");

if ( r0 + rowL > m_rows || c0 + colL > m_cols )
errMsg("swdMat :: setSlice() , range out of range!");
#endif

for (unsigned int i=r0; i<r0+rowL; i++)
for (unsigned int j=c0; j<c0+colL; j++)
m_buf[i*m_cols + j] = val;
}

void swdMat :: setSlice( const unsigned int r0, const unsigned int c0, //top left position
const unsigned int rowL, const unsigned int colL,        // row and col length
const swdMat &x )
{
#if SW_MAT_BOUNDS_CHECK
if ( r0 < 0 || c0 < 0 )
errMsg("swdMat :: setSlice() , left top position is out of range!");

if ( rowL <= 0 || colL <= 0 )
errMsg("swdMat :: setSlice() , the height and width is out of range!");

if ( r0 + rowL > m_rows || c0 + colL > m_cols )
errMsg("swdMat :: setSlice() , range out of range!");
#endif

unsigned int k=0;
for (unsigned int i=r0; i<r0+rowL; i++)
for (unsigned int j=c0; j<c0+colL; j++)
m_buf[i*m_cols + j] = x.m_buf[k++];
}



swdMat& swdMat :: randomize()
{
for ( unsigned int k=0; k<m_size; k++ )
m_buf[k] = (double)rand() / (double)RAND_MAX;

return *this;
}

swdMat swdMat :: transpose() const
{
swdMat y(m_cols, m_rows);
for ( unsigned int i=0; i<m_rows; i++ )
for ( unsigned int j=0; j<m_cols; j++ )
y.m_buf[j*m_rows + i ]	= m_buf[i*m_cols + j ];

return y;
}


//getSlice for the matrix is a column vector
swdMat swdMat :: getSubCol( const unsigned int c, const unsigned int r0, const unsigned int rowL ) const
{
swdMat y;
if( m_size == 0 ){
return y;
}

#if SW_MAT_BOUNDS_CHECK
if ( c < 0 || c >= m_cols )
errMsg("swdMat :: getSubCol(), top row position is out of range!");
if ( r0 < 0 || rowL <= 0)
errMsg("swdMat :: getSubCol(), start position or height is out of range!");
if ( r0 + rowL > m_rows  )
errMsg("swdMat :: getSubCol(), index is out of range!");
#endif

y.resize(rowL, 1);
const double *px= m_buf + r0*m_cols + c;
double *py= y.getBuf();
for (unsigned int i=0; i<rowL; ++i, px+=m_cols )
*py++ = *px;

return y;
}


//delete the c-th column
void swdMat :: delCol( const unsigned int c )
{
delCols( c, c );
}

//delete the c-th column
void swdMat :: delCols( const unsigned int c1, const unsigned int c2, const unsigned int skip )
{
#if SW_MAT_BOUNDS_CHECK
if ( c2 >= m_cols )
errMsg("swdMat :: delCols(), c2 is out of range!");

if ( c1 < 0 )
errMsg("swdMat :: delCols(), c1 is out of range!");
#endif
if ( c1 == 0 && c2 == (m_cols-1) && skip==1 ){
resize(0, 0);
return;
}

swdMat x0( *this ); //copy the current matrix

unsigned int deletedCols = (c2-c1)/skip + 1;
unsigned int delIdSize = deletedCols * m_rows;
unsigned int* delIdBuf = new unsigned int[delIdSize];

unsigned i, j, k=0;
for(i=0; i<x0.rows(); i++)
for(j=c1; j<=c2; j += skip)
delIdBuf[k++] = i*m_cols + j;

//create the matrix after deleting
unsigned int newCols = m_cols - deletedCols;
resize(m_rows, newCols);

//put the filtered data in m_buf[]
delElementsFromBuf(x0.m_buf, x0.size(), delIdBuf, delIdSize, m_buf);

delete [] delIdBuf;
}

//delete the r-th row
void  swdMat :: delRow( const unsigned int r )
{
delRows(r, r);
}

void swdMat :: delRows( const unsigned int r1, const unsigned int r2, const unsigned int skip )
{
#if SW_MAT_BOUNDS_CHECK
if ( r1 > r2 )
errMsg("swdMat :: delRows(), r1 must be smaller than or equal r2!");
if ( r1 < 0 )
errMsg("swdMat :: delRows(), r1 is out of range!");
#endif
if ( r1 == 0 && r2 == (m_rows-1) && skip==1 ) //try to delete all
{
resize(0, 0);
return;
}
if( r1 >= m_rows ) return;

unsigned int rr2 = ( r2 >= m_rows ) ? (m_rows-1) : r2;

swdMat x0( *this ); //copy the current matrix

unsigned int deletedRows = (rr2-r1)/skip + 1;
unsigned int delIdSize = deletedRows * m_cols;
unsigned int* delIdBuf = new unsigned int[delIdSize];

unsigned int i, j, k=0;
for( i=r1; i<=rr2; i+=skip )
for( j=0; j<m_cols; j++ )
delIdBuf[k++] = i*m_cols + j;

//create the matrix after deleting
unsigned int newRows = m_rows - deletedRows;
resize(newRows, m_cols);

//put the filtered data in m_buf[]
delElementsFromBuf(x0.m_buf, x0.size(), delIdBuf, delIdSize, m_buf);

delete [] delIdBuf;
}

// insert a column with same val
void  swdMat :: insertCol( const unsigned int c, const double val ) 
{
swdMat v(m_rows, 1, val);
insertCol( c, v );
}

// insert a column with data from matrix x
void  swdMat :: insertCol( const unsigned int c, const swdMat &x )    
{
swdMat x0( *this ); //copy the current matrix
#if SW_MAT_BOUNDS_CHECK
if ( c<0 )
errMsg( "swdMat :: insertCol(): column # is out of range!"); 

if ( x.cols() != 1 ) 
errMsg( "swdMat :: insertCol(): x is not a column vector!"); 

if ( x0.rows() != x.rows() )
errMsg( "swdMat :: insertCol(): # of rows are miss matching!"); 
#endif

if ( c == 0  ) //insert at the beginging
{
resize(m_rows, x0.cols() + 1);
setCol( 0, x );
setSlice( 0, 1,	m_rows, x0.cols(), x0 );  
}
else if ( c < x0.cols() ) //insert in beween 
{
unsigned int deltaC = m_cols - c;
swdMat s1 = x0.getSlice(0, 0, m_rows, c);
swdMat s2 = x0.getSlice(0, c, m_rows, deltaC);

resize(m_rows, x0.cols() + 1);

setSlice(0, 0, m_rows, c, s1);
setCol(c, x);
setSlice(0, c+1, m_rows, deltaC, s2); 
}
else //insert in outside
{
resize(m_rows, c+1);
setSlice( 0, 0,	m_rows, x0.cols(), x0 );  
//NOTE! there are some junk data in bewteen
setCol( c, x );
}
}

// insert a row with same data value
void  swdMat :: insertRow( const unsigned int r, const double val ) 
{
swdMat v(1, m_cols, val);
insertRow( r, v );
}

void swdMat :: insertRow( const unsigned int r, const double* buf )
{
swdMat v(1, m_cols, buf, m_cols);
insertRow( r, v );
}

// insert a row with data from matrix x
void  swdMat :: insertRow( const unsigned int r, const swdMat& x )
{
swdMat x0( *this ); //copy the current matrix

#if SW_MAT_BOUNDS_CHECK
if ( r<0 )
errMsg( "swdMat :: insertRow(): row # is out of range!"); 

if ( x.rows() != 1 ) 
errMsg( "swdMat :: insertRow(): x is not a row matrix!"); 

if ( x0.cols() != x.cols() )
errMsg( "swdMat :: insertrow(): # of columns are miss matching!"); 
#endif

if ( r == 0  ) //insert at the beginging
{
resize( x0.rows()+1, x0.cols() );
setRow( 0, x );
setSlice( 1, 0,	x0.rows(), m_cols, x0 );  
}
else if ( r < x0.rows() ) //insert in beween 
{
unsigned int deltaR = m_rows - r;
swdMat s1 = x0.getSlice(0, 0, r, m_cols);
swdMat s2 = x0.getSlice(r, 0, deltaR, m_cols);

resize(x0.rows() + 1, m_cols);

setSlice(0, 0, r, m_cols, s1);
setRow(r, x);
setSlice(r+1, 0, deltaR, m_cols, s2); 
}
else //insert in outside
{
resize(r+1, m_cols);
setSlice( 0, 0,	x0.rows(), m_cols, x0 );  
//NOTE! there are some junk data in bewteen
setRow( r, x );
}
}

//keep the original values at the original place, but extened number of rows
//and the garbage are in the extended places
void swdMat :: extendRows( const unsigned int numNewRows )
{
#if SW_MAT_BOUNDS_CHECK
if (m_rows>= numNewRows){
errMsg("swdMat :: extendRows(): numNewRows <= current # of rows!");
}
#endif
swdMat x0( *this ); //copy the current matrix

resize( numNewRows, m_cols);
setSlice(0,0,x0.rows(),x0.cols(), x0);
}

swdMat swdMat :: getSubRow( const unsigned int r, const unsigned int c0, const unsigned int colL ) const
{
swdMat y;
if( m_size == 0 ){
return y;
}

#if SW_MAT_BOUNDS_CHECK
if ( r < 0 || r >= m_rows )
errMsg("swdMat :: getSubRow(), column position is out of range!");

if ( c0 < 0 || c0 >=m_cols )
errMsg("swdMat :: getSubRow(), start position or length is out of range!");

if ( c0 + colL > m_cols  )
errMsg("swdMat :: getSubRow(), index is out of range!");

if ( colL <0 || colL >m_cols  )
errMsg("swdMat :: getSubRow(), index is out of range!");
#endif

y.resize(1, colL);
double *py = y.getBuf();
const double *px = m_buf + r*m_cols + c0;
for (unsigned int j=0; j<colL; ++j )
*py++ = *px++ ;

return y;
}

swdMat swdMat :: getCol( const unsigned int c ) const
{
return getSubCol(c, 0, m_rows);
}

swdMat swdMat :: getCols( const unsigned int cBeg, const unsigned int cEnd, const unsigned int nSkip ) const
{
#if SW_MAT_BOUNDS_CHECK
if ( cBeg<0 || cEnd >= m_cols )
errMsg( "swdMat :: getCols(): bundary out of range!");
#endif

unsigned int nCols = 1 + (cEnd - cBeg) / nSkip;
swdMat x(m_rows, nCols);
unsigned int k=0;
for (unsigned int c=cBeg; c<=cEnd; c += nSkip){
x.setCol(k, getCol(c) );
k++;
}

return x;
}

double* swdMat :: getRowAddress( const unsigned int &r )
{
return m_buf + r*m_cols;
}

const double* swdMat :: getRowAddress2( const unsigned int &r ) const
{
return m_buf + r*m_cols;
}

swdMat swdMat :: getRow( const unsigned int r ) const
{
return getSubRow( r, 0, m_cols );
}

swdMat swdMat :: getRows( const unsigned int rBeg, const unsigned int rEnd, const unsigned int nSkip ) const
{
#if SW_MAT_BOUNDS_CHECK
if ( rBeg<0 || rEnd >= m_rows )
errMsg( "swdMat :: getRows(): bundary out of range!");
#endif

unsigned int nRows = 1 + (rEnd - rBeg) / nSkip;
swdMat x(nRows, m_cols);
unsigned int k=0;
for (unsigned int r=rBeg; r<=rEnd; r += nSkip){
x.setRow(k, getRow(r) );
k++;
}

return x;
}

swdMat swdMat :: getSlice( const unsigned int r0, const unsigned int c0,               //top left position
const unsigned int rowL, const unsigned int colL ) const    // row and col length 
{
#if SW_MAT_BOUNDS_CHECK
if ( r0 < 0 || c0 < 0 )
errMsg("swdMat :: getSlice() , left top position is out of range!");

if ( rowL <= 0 || colL <= 0 )
errMsg("swdMat :: getSlice() , the height and width is out of range!");

if ( r0 + rowL > m_rows || c0 + colL > m_cols )
errMsg("swdMat :: getSlice() , range out of range!");
#endif

int k=0;
swdMat y(rowL, colL);
for (unsigned int i=r0; i<r0+rowL; i++)
for (unsigned int j=c0; j<c0+colL; j++)
y.m_buf[k++] = m_buf[i*m_cols + j];

return y;
}

// return a  min(m_rows, m_cols) x 1 matrix which contains the diagonal elements of *this
swdMat swdMat :: getDiagonal() const
{
unsigned int m = SW_MAT_MIN(m_rows, m_cols);
swdMat y(m, 1);
for (unsigned int j=0;j<m; j++)
y.m_buf[j] = m_buf[ j*m_cols + j] ;

return y;
}

//--------------------------------------------
// the following are class private functions
//--------------------------------------------
// param $size$ is not neccessary, here just for safty consideration
void swdMat :: deepCopy( double* buf, const unsigned int size ) const
{
#if SW_MAT_BOUNDS_CHECK
if ( buf == NULL )
errMsg( "Error: swdMat::deepCopy() -- buf is NULL!" );

if ( size != m_size )
errMsg( "Error: swdMat::deepCopy() -- size not match the m_size!" );
#endif

for( unsigned k=0; k<m_size; k++)
buf[k] = m_buf[k];
}

void swdMat::creatBuf()
{
if (m_size > 0){
if (m_buf)
deleteBuf();

m_buf = new double[m_size];

if ( !m_buf )
errMsg( "swdMat::creatBuf(): cannot allocate memory!" );
}
else{
m_size = 0;
m_rows = 0;
m_cols = 0;
m_buf = NULL;
}
}
void swdMat::deleteBuf()
{
if ( m_buf ){
delete [] m_buf;
m_buf = NULL;
}
}

//-----------------------------------------------------------------------------
//input params
//  oldBuf[],original data buffer
//  oldSize, the elements of oldBuf[]
//  deletedIdBuf[], all the elements in oldBuf[] with the IDs stored in deletedIdBuf[] will be deleted
//                  these elements are stored in the order of from smallest to largest
//  deletedIdBufSize, the elements of deletedIdBuf[]
//output
//  newBufp[], the reminder elements oldBuf[] still keep their original order
//-----------------------------------------------------------------------------
void swdMat :: delElementsFromBuf(const double* oldBuf, const unsigned int oldSize, 
const unsigned int* deletedIdBuf, const unsigned int deletedIdBufSize, double* newBuf)
{
bool isFound;
unsigned int i, j, k=0;
unsigned int n0=0;  //start search index in deletedIdBuf[]
for( i=0; i<oldSize; i++){
//find index i in deletedIdBuf
isFound = false;
for (j=n0; j<deletedIdBufSize; j++){
if (i == deletedIdBuf[j]){
isFound = true;
n0 = j+1;
break;
}
}

if ( !isFound )
newBuf[k++] = oldBuf[i];
}
}

void swdMat :: writeToFile( const char* fileName, const char *fmt ) const
{
vector<string> tLines; 
tLines.clear();

writeToFile( fileName, tLines, fmt );
}

void swdMat :: writeToFile( const char* fileName, const vector<string> &titleLines, const char *fmt ) const
{

string fmt1,fmt2;
if( !fmt ){
fmt1="%f ";
fmt2="%f\n";
}
else{
fmt1 = string (fmt) + " ";
fmt2 = string (fmt) + "\n";
}

FILE *fp = fopen( fileName, "w+" ) ;

if( fp==NULL ) {
errMsg ( "writeEstimationToFile(): file  cannot be opend!" );
}

//write the title lines
for ( unsigned int i=0; i<titleLines.size(); i++ )
{
fprintf(fp, "%s\n", titleLines[i].c_str() );
}

//write data
for ( unsigned int i=0; i<m_rows; i++ )
{
for (unsigned int j=0; j<m_cols-1; j++)
fprintf(fp, fmt1.c_str(), m_buf[ i*m_cols+j ] );

fprintf(fp, fmt2.c_str(), m_buf[ i*m_cols + m_cols-1 ] );
}

fclose( fp );
}


void swdMat :: appendToFile( FILE *fp, char *fmtStr, char *msgStr, bool isPrintSize ) const
{
if ( msgStr )
fprintf(fp, "%s\n", msgStr );

if ( isPrintSize )
fprintf(fp, "%d %d\n", m_rows, m_cols );

unsigned int i, j, k=0;
for ( i=0; i<m_rows; i++ )
{
for ( j=0; j<m_cols-1; j++){
fprintf(fp, "%f ", m_buf[ k++ ] );
}

fprintf(fp, "%f\n", m_buf[ k++ ] );
}
}

void swdMat :: readFromFile( const unsigned int nCols, const char* fileName, const int nMaxLines )
{
readFromFile( fileName, 0, nCols,  nMaxLines);
}

void swdMat :: readFromFile( const char* fileName, const unsigned int numOfLinesSkiped, const unsigned int nCols,  const long nMaxLines)
{
unsigned int i, k=0;
double tmp;
unsigned int nTryRows;
swdMat buf(1,nCols);

if (nMaxLines == -1) //default read all
nTryRows = 1000;
else
nTryRows = nMaxLines;

resize( nTryRows, nCols );

FILE *fp = fopen( fileName, "r" ) ;
if( fp == NULL ) {
cout <<"file: " << fileName << endl;
errMsg("swdMat :: readFromFile(): file  cannot be opend!");
return;
}

//read the title lines, and discharge them
int nMaxChars = 1024;
char line[1024];
for(i=0; i<numOfLinesSkiped; i++){
fgets( line, nMaxChars, fp );
//cout << line <<endl;
}

bool breakFlag = false;
while ( 1 )
{
for (i=0; i<nCols; i++){
if (nMaxLines == -1) //read all
{
if ( feof(fp) ){
breakFlag = true;
break;
}
}
else //read nMaxLines
{
if ( feof(fp) || k>= nTryRows ){
breakFlag = true;
break;
}
}

fscanf( fp, "%lf", &tmp);
buf(i) = tmp;
};

if ( breakFlag ) break;

if( k<nTryRows ){
setRow(k, buf);	
}
else{
insertRow( k, buf );
}
k++;
}
fclose (fp );

if ( k>0 && k<nTryRows ){
delRows( k, nTryRows-1 );
}

if ( k==0 ){
delRows( 1, nTryRows-1 ); //only keep the 1st row to save the memory
warningMsg("swdMat :: readFromFile(): There is no data in the file, a junk matrix (1 by n) is returned!");
}
}


void swdMat :: readFromFile2( const std::string &path, const std::string &fname_wo_ext, const int &fn, const int &idx)
{
char fileName[1024];
if( idx<0 )
sprintf_s( fileName, 1024, "%s//%s-%05d.txt", path.c_str(), fname_wo_ext.c_str(), fn );
else
sprintf_s( fileName, 1024, "%s//%s-%05d-%05d.txt", path.c_str(), fname_wo_ext.c_str(), fn, idx );


FILE *fp = fopen( fileName, "r" ) ;
if( fp == NULL ) {
cout <<"file: " << fileName << endl;
errMsg("swdMat :: readFromFile2(): file  cannot be opend!");
return;
}

int i,j,m,n;
fscanf(fp, "%d,%d", &m, &n );

resize(m,n);

double *p = m_buf;
double tmp;
for (j=0; j<m; ++j){
for (i=0; i<n-1; ++i){
fscanf( fp, "%lf,", &tmp);
*p++ = tmp;
}
fscanf( fp, "%lf", &tmp);
*p++ = tmp;
}
fclose (fp );
}


void swdMat::writeToFile2( const std::string &path, const std::string &fname_wo_ext, const std::string &fmt, const int &fn, const int &idx)
{
int m =  rows();
int n =  cols();

char fileName[1024];
if( idx<0 )
sprintf_s( fileName, 1024, "%s\\%s-%05d.txt", path.c_str(), fname_wo_ext.c_str(), fn );
else
sprintf_s( fileName, 1024, "%s\\%s-%05d-%05d.txt", path.c_str(), fname_wo_ext.c_str(), fn, idx );

std::string fmt1 = fmt + ",";
std::string fmt2 = fmt + "\n";

FILE *fp = fopen( fileName, "w" );
if( fp == NULL ) {
printf("my_text_writer2(): file: \n %s \n  cannot be opened!\n", fileName);
return;
}
const double *px = data();
int i,j;
fprintf(fp, "%d,%d\n", m, n );
for (j=0; j<m; ++j){  //row
for (i=0; i<n-1; ++i){  //col
fprintf( fp, fmt1.c_str(), *px++ );
}
fprintf( fp, fmt2.c_str(), *px++ );
}

fclose (fp );
}

void swdMat::errMsg ( const char* msgStr) const
{
//debug version
cerr << msgStr << endl; 
getchar();

//release version
throw runtime_error( msgStr );
}

void swdMat::warningMsg ( const char* msgStr) const
{
//debug version
cerr << msgStr << endl; 
getchar();
}
