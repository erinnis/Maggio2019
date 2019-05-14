#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void leggimat(char *nomefile,int *M,int dim1,int dim2);
void stampamat(int *M,int dim1,int dim2);
void stampavet(int *V,int dim);




void main(int argc,char ** argv)
{
    int nproc,my_id;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&nproc);
    MPI_Comm_rank(MPI_COMM_WORLD,&my_id);
    int dim=16; //dim pari nproc 4 k 4
    int k=dim/nproc;
    int A[dim][dim];

    MPI_Datatype k_column;
    MPI_Datatype k_column_resized;
    MPI_Type_vector(dim,1,dim,MPI_INT,&k_column);
    MPI_Type_create_resized(k_column,0,k*sizeof(int),&k_column_resized);
    MPI_Type_commit(&k_column_resized);
    int T[k][dim];
    MPI_Status status;


    if(my_id==0)
    {
        leggimat("mat.txt",&A[0][0],dim,dim);
        stampamat(&A[0][0],dim,dim);
        for(int i=0;i<nproc;i++)
        {
            MPI_Send(&A[0][k*i],k,k_column_resized,i,99,MPI_COMM_WORLD);
        }
    }
    MPI_Recv(&T[0][0],k*dim,MPI_INT,0,99,MPI_COMM_WORLD,&status);
    MPI_Barrier(MPI_COMM_WORLD);
    printf("\n");
    if(my_id==0)
    stampamat(&T[0][0],k,dim);

    int V[k];

    if(my_id==0)
    {
        for(int i=0;i<k;i++)
        {
            V[i]=rand()%100;

        }
        stampavet(&V[0],k);

    }
    MPI_Barrier(MPI_COMM_WORLD);
    printf("\n");


            if(my_id!=0)
            {
                MPI_Recv(&V[0],k,MPI_INT,my_id-1,90,MPI_COMM_WORLD,&status);
                V[my_id]=rand()%V[my_id-1];
            }
            if(my_id!=nproc-1)
            {
                MPI_Send(&V[0],k,MPI_INT,my_id+1,90,MPI_COMM_WORLD);
            }
           
                MPI_Barrier(MPI_COMM_WORLD);
    printf("\n");
    
        if(my_id==1)
        {
            stampavet(&V[0],k);
        }
        //V[0][k]*T[k][dim]=S[dim]
        int S[dim];
        int somma=0;

        for(int i=0;i<dim;i++)
        {
            for(int j=0;j<k;j++)
            {
                somma+=V[j]*T[j][i];
            }
            S[i]=somma;
            somma=0;
        }
        
        if(my_id==0)
        {
            stampavet(&S[0],dim);
        }
        int color=0;
        if(my_id%2==1)
        {
            color=1;
        }
        int pari[dim/2];
        int dispari[dim/2];
        int i,j;
        if(color==0)
        {   
            i=j=0;
            do
            {
                pari[i]=S[j];
                i++;
                j=j+2;
            } while (j<dim);
        }
        else 
        {   i=0;
            j=1;
            do
            {
                dispari[i]=S[j];
                i++;
                j=j+2;
            } while (j<dim);
        }
        MPI_Comm newcomm;

        MPI_Comm_split(MPI_COMM_WORLD,color,my_id,&newcomm);
        int massimi[dim/2];
        int minimi[dim/2];
        
        if(color==0) //pari quindi massimo
        {
            MPI_Allreduce(&pari[0],&massimi[0],dim/2,MPI_INT,MPI_MAX,newcomm);
        }
        else
        {
            MPI_Allreduce(&dispari[0],&minimi[0],dim/2,MPI_INT,MPI_MIN,newcomm);
        }

        if(color==1)
        {
            stampavet(&dispari[0],dim/2);
        }
        MPI_Barrier(newcomm);
        printf("\n");
        MPI_Barrier(newcomm);

        if(color==1 && my_id==1)
        {
            stampavet(&minimi[0],dim/2);
        }


    MPI_Finalize();

}

void leggimat(char *nomefile,int *M,int dim1,int dim2)
{
    FILE *f =fopen(nomefile,"r");
    for(int i=0;i<dim1;i++)
    {
        for(int j=0;j<dim2;j++)
        {
            fscanf(f,"%d",&M[i*dim2+j]);
        }
    }
}

void stampamat(int *M,int dim1,int dim2)
{   printf("\n");
    for(int i=0;i<dim1;i++)
    {
        for(int j=0;j<dim2;j++)
        {
            printf("%d ",M[i*dim2+j]);
        }   printf("\n");
    }
    printf("\n");
}

void stampavet(int *V,int dim)
{
    for(int i=0;i<dim;i++)
    {
        printf("%d ",V[i]);
    }
    printf("\n");
}