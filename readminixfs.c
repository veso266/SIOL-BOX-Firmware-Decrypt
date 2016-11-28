#include "readminixfs.h"
  
 char errormsg[1024];    /* global variable, but used only for error output */
+u_long block_size=BLOCK_SIZE;
+u_long inode_per_block_v1=BLOCK_SIZE/sizeof(mx_inode_disk);
+u_long inode_per_block_v2=BLOCK_SIZE/sizeof(mx2_inode_disk);
  
 static u_short swap_s(u_short us)
 {

 }
 #endif
  

static int readBlock(mx_device *pDev, int fd, int blocknum, u_char *dst, int size)
 {
     int res, nbread;
  

         memset(dst, 0, size);
         return 0;
     }
    if (blocknum>1)
        res = lseek(fd, blocknum * pDev->super->s_blocksize, SEEK_SET);
    else
         res = lseek(fd, 1024, SEEK_SET);       
     if (res < 0) return res;
     nbread = 0;
     while (nbread < size) {

     mx_sb_disk phys_sb;
     mx_sb_info *log_sb;
  
    log_sb = malloc(sizeof(mx_sb_info));
    if (log_sb == NULL)
        goto fail;
    log_sb->s_blocksize=0;
    pDev->super = log_sb;
    if (readBlock(pDev, pDev->fd, 1, (u_char*) &phys_sb, sizeof(mx_sb_disk))) {
         snprintf(errormsg, sizeof(errormsg), "can\'t read superblock");
         goto fail;
     }

         && magic != MINIX_SUPER_MAGIC2_COMPR
         && magic != MINIX_SUPER_MAGIC2_COMPR_REV
         && magic != MINIX2_SUPER_MAGIC2_COMPR
        && magic != MINIX2_SUPER_MAGIC2_COMPR_REV 
        && magic != MINIX25_SUPER_MAGIC2_COMPR
        && magic != MINIX25_SUPER_MAGIC2_COMPR_REV
        && magic != MINIX25_SUPER_MAGIC2_COMPR
        && magic != MINIX25_SUPER_MAGIC2_COMPR_REV )
        {
         snprintf(errormsg, sizeof(errormsg), "superblock : unknown magic");
         goto fail;
     }
     log_sb->inode_map = NULL;
     log_sb->zone_map = NULL;
     log_sb->magic = magic;

         || magic == MINIX_SUPER_MAGIC2_COMPR
         || magic == MINIX_SUPER_MAGIC2_COMPR_REV
         || magic == MINIX2_SUPER_MAGIC2_COMPR

        || magic == MINIX2_SUPER_MAGIC2_COMPR_REV 
        || magic == MINIX25_SUPER_MAGIC2
        || magic == MINIX25_SUPER_MAGIC2_COMPR     
        || magic == MINIX25_SUPER_MAGIC2_REV
        || magic == MINIX25_SUPER_MAGIC2_COMPR_REV       
        )
         log_sb->long_name = 1;
     else
         log_sb->long_name = 0;
     if (magic == MINIX_SUPER_MAGIC_REV || magic == MINIX_SUPER_MAGIC2_REV
         || magic == MINIX2_SUPER_MAGIC_REV || magic == MINIX2_SUPER_MAGIC2_REV
         || magic == MINIX_SUPER_MAGIC2_COMPR_REV
        || magic == MINIX2_SUPER_MAGIC2_COMPR_REV
        || magic == MINIX25_SUPER_MAGIC2_REV
        || magic == MINIX25_SUPER_MAGIC2_COMPR_REV    
        )
         log_sb->must_swap = 1;
     else
         log_sb->must_swap = 0;
     if (magic == MINIX_SUPER_MAGIC2_COMPR
         || magic == MINIX_SUPER_MAGIC2_COMPR_REV
         || magic == MINIX2_SUPER_MAGIC2_COMPR
        || magic == MINIX2_SUPER_MAGIC2_COMPR_REV 
        || magic == MINIX25_SUPER_MAGIC2_COMPR 
        || magic == MINIX25_SUPER_MAGIC2_COMPR_REV          
        )
         log_sb->compressed = 1;
     else
         log_sb->compressed = 0;
  
     swap = log_sb->must_swap;
     if (swap) {
         log_sb->nb_inode = swap_s(phys_sb.nb_inode);

         log_sb->block_per_zone = 1 << swap_s(phys_sb.log_zone_size);
         log_sb->max_file_size = swap_l(phys_sb.max_file_size);
         log_sb->clean = swap_s(phys_sb.clean);
        if (log_sb->minix_version >=2)
        {
            log_sb->s_blocksize=swap_s(phys_sb.s_blocksize);   
            inode_per_block_v2 = log_sb->s_blocksize/sizeof(mx2_inode_disk);
            printf("sBlock size is: %d\n", log_sb->s_blocksize);
           printf("Inodes per block: %d\n", inode_per_block_v2);
        }
        else
          log_sb->s_blocksize = BLOCK_SIZE; 
     } else {
         log_sb->nb_inode = phys_sb.nb_inode;
         log_sb->nb_zone = phys_sb.nb_zone;

         log_sb->block_per_zone = 1 << phys_sb.log_zone_size;
         log_sb->max_file_size = phys_sb.max_file_size;
         log_sb->clean = phys_sb.clean;
        if (log_sb->minix_version >=2)
        {
            log_sb->s_blocksize=phys_sb.s_blocksize;  
            inode_per_block_v2 = log_sb->s_blocksize/sizeof(mx2_inode_disk);            
            printf("Block size is: %d\n", log_sb->s_blocksize);
            printf("Inodes per block: %d\n", inode_per_block_v2);   
        }
        else
           log_sb->s_blocksize = BLOCK_SIZE; 
     }
     /* and now, copy maps */
    printf("b_inode map: %d nb_zone_map: %d \n", log_sb->nb_inode_map, log_sb->nb_zone_map);
    map = malloc(log_sb->nb_inode_map * log_sb->s_blocksize);
     if (map == NULL)
         goto fail;
     for (i=0 ; i < log_sb->nb_inode_map ; i++)

        if (readBlock(pDev, pDev->fd, 2 + i, map + (i * log_sb->s_blocksize), log_sb->s_blocksize))
             goto fail;
     log_sb->inode_map = map;

    map = malloc(log_sb->nb_zone_map * log_sb->s_blocksize);
     if (map == NULL)
         goto fail;
     for (i=0 ; i < log_sb->nb_zone_map ; i++)

        if (readBlock(pDev, pDev->fd, 2 + log_sb->nb_inode_map + i, 
            map + (i * log_sb->s_blocksize), log_sb->s_blocksize))
             goto fail;
     log_sb->zone_map = map;
     return 0;

 static int initInode(mx_device *pDev, mx_inode_info *inode)
 {
     int inode_num, block_num, offset, nb_blockid_per_block, i, nb;

    u_char block[BLOCK_SIZE*8];  /* max block size  */
     mx_inode_disk diskInode;
    mx2_inode_disk diskInode2;
  
     inode->data_zones = NULL;
     inode->indirect_zones = NULL;

             inode_num);
     goto fail;
     }
    if (pDev->super->minix_version >=3) {  /* Minix V3 inode unsupported */
         snprintf(errormsg, sizeof(errormsg), "bad version of minix FS");
     goto fail;
     }

     /* find the block number of this inode */
     block_num = 2 + pDev->super->nb_inode_map + pDev->super->nb_zone_map;
     /* first inode in first inode block is number one, not zero */
    if (pDev->super->minix_version==1) 
        block_num += (inode_num -1) / inode_per_block_v1;
    else
        block_num += (inode_num -1) / inode_per_block_v2;  
    if (readBlock(pDev, pDev->fd, block_num, block, pDev->super->s_blocksize))
       goto fail;
    if (pDev->super->minix_version==1)    
        offset = ((inode_num -1) % inode_per_block_v1) * sizeof(mx_inode_disk);
    else
        offset = ((inode_num -1) % inode_per_block_v2) * sizeof(mx2_inode_disk);     

    if (pDev->super->minix_version==1)     
        memcpy(&diskInode, block+offset, sizeof(mx_inode_disk));
    else
       memcpy(&diskInode2, block+offset, sizeof(mx2_inode_disk));        

    if (pDev->super->minix_version==1)    
    {    
        if (pDev->super->must_swap) {
            inode->mode = swap_s(diskInode.mode);
            inode->uid = swap_s(diskInode.uid);
            inode->size = swap_l(diskInode.size);
            inode->time = swap_l(diskInode.time);
            inode->gid = swap_s(diskInode.gid);
        inode->nb_link = diskInode.nb_link;
            for (i = 0 ; i < 9 ; i++)
                inode->raw_zone[i] = swap_s(diskInode.zone[i]);
        } else {
        inode->mode = diskInode.mode;
        inode->uid = diskInode.uid;
        inode->size = diskInode.size;
        inode->time = diskInode.time;
        inode->gid = diskInode.gid;
        inode->nb_link = diskInode.nb_link;
       for (i = 0 ; i < 9 ; i++)
            inode->raw_zone[i] = diskInode.zone[i];
        }
   }
    else{
        if (pDev->super->must_swap) {
            inode->mode = swap_s(diskInode2.mode);
            inode->uid = swap_s(diskInode2.uid);
            inode->size = swap_l(diskInode2.size);
            inode->time = swap_l(diskInode2.atime);
            inode->gid = swap_s(diskInode2.gid);
        inode->nb_link = swap_s(diskInode2.nlinks);
            for (i = 0 ; i < 10 ; i++)
                inode->raw_zone[i] = swap_s(diskInode2.zone[i]);
       } else {
        inode->mode = diskInode2.mode;
        inode->uid = diskInode2.uid;
        inode->size = diskInode2.size;
       inode->time = diskInode2.atime;
        inode->gid = diskInode2.gid;
        inode->nb_link = diskInode2.nlinks;
        for (i = 0 ; i < 10 ; i++)
            inode->raw_zone[i] = diskInode2.zone[i];
        }    
    }
     
     /* compressed file */
     if (pDev->super->compressed && 
         (inode->uid & MINIX_FLAGS_MASK) == MINIX_COMPRBLK_4KB) {

     if (inode->size == 0) /* empty file, nothing else to do */
         return 0;
  
    inode->nb_data_zone = inode->size / pDev->super->s_blocksize;
    if (inode->size % pDev->super->s_blocksize> 0)
     inode->nb_data_zone++;
     inode->data_zones = malloc(inode->nb_data_zone * sizeof(int));
     if (inode->data_zones == NULL)

        goto fail;

     nb = (7 < inode->nb_data_zone) ? 7 : inode->nb_data_zone;
     for (i = 0 ; i < nb ; i++)
     inode->data_zones[i] = inode->raw_zone[i];
     if (inode->nb_data_zone <= 7)

        return 0;

  
     /* indirect block(s) */

    if (pDev->super->minix_version==1)
        nb_blockid_per_block = pDev->super->s_blocksize / sizeof(u_short);
    else
        nb_blockid_per_block = pDev->super->s_blocksize / sizeof(u_long);      
     /* count all not-data (simple or double indirect) blocks */
     /* put result into nb */
     if (inode->nb_data_zone <= (nb_blockid_per_block + 7)) {

     }
     inode->indirect_zones = malloc(nb * sizeof(int));
     if (inode->indirect_zones == NULL)
        goto fail;

     inode->nb_indirect_zone = nb;
     inode->indirect_zones[0] = inode->raw_zone[7];
  

    if (readBlock(pDev, pDev->fd, inode->raw_zone[7], block, pDev->super->s_blocksize))
        goto fail;
    
    if (pDev->super->minix_version==1){
        for (i = 0 ; i < (int)(pDev->super->s_blocksize / sizeof(u_short)) ; i++) {
        u_short us;
        nb = i + 7;
       if (nb == inode->nb_data_zone)
            return 0;
        memcpy(&us, block + 2*i, sizeof(u_short));
        if (pDev->super->must_swap)
            inode->data_zones[nb] = (u_long) swap_s(us);
        else
           inode->data_zones[nb] = (u_long) us;
        }
    }
    else{
        for (i = 0 ; i < (int)(pDev->super->s_blocksize / sizeof(u_long)) ; i++) {
       u_long us;
        nb = i + 7;
        if (nb == inode->nb_data_zone)
            return 0;
        memcpy(&us, block + 4*i, sizeof(u_long));
       if (pDev->super->must_swap)
            inode->data_zones[nb] = (u_long) swap_l(us);
       else
            inode->data_zones[nb] = (u_long) us;
        }    
    }
  
     /* double indirect block */
     inode->indirect_zones[1] = inode->raw_zone[8];
    if (readBlock(pDev, pDev->fd, inode->raw_zone[8], block, pDev->super->s_blocksize))
       goto fail;
  
    nb = 7 + nb_blockid_per_block; /* nb holds the current data block num */
    
    if (pDev->super->minix_version==1){
        for (i = 0 ; i < nb_blockid_per_block ; i++) {
        u_short us1, us2;
        u_char ind_block[BLOCK_SIZE*8];
        int zone_num, j;

        memcpy(&us1, block+ 2*i, sizeof(u_short));
        if (pDev->super->must_swap)
            zone_num = swap_s(us1);
        else
            zone_num = us1;
        inode->indirect_zones[2+i] = zone_num;
        if (readBlock(pDev, pDev->fd, zone_num, ind_block, pDev->super->s_blocksize))
            goto fail;

        for (j = 0 ; j < pDev->super->s_blocksize / 2 ; j++) {
           if (nb == inode->nb_data_zone)
                return 0;
            memcpy(&us2, ind_block + 2*j, sizeof(u_short));
            if (pDev->super->must_swap)
               zone_num = swap_s(us2);
            else
                zone_num = us2;
           inode->data_zones[nb++] = zone_num;
        }        
        }
    }
    else{
        for (i = 0 ; i < nb_blockid_per_block ; i++) {
        u_long us1, us2;
       u_char ind_block[BLOCK_SIZE*8];
        u_long zone_num, j;

        memcpy(&us1, block+ 4*i, sizeof(u_long));
        if (pDev->super->must_swap)
           zone_num = swap_l(us1);
        else
            zone_num = us1;
        inode->indirect_zones[2+i] = zone_num;
        if (readBlock(pDev, pDev->fd, zone_num, ind_block, pDev->super->s_blocksize))
            goto fail;

        for (j = 0 ; j < pDev->super->s_blocksize / 4 ; j++) {
            if (nb == inode->nb_data_zone)
                return 0;
            memcpy(&us2, ind_block + 4*j, sizeof(u_long));
            if (pDev->super->must_swap)
               zone_num = swap_l(us2);
            else
                zone_num = us2;
            inode->data_zones[nb++] = zone_num;
        }        
        }        
    }
    
     return 0;
  
 fail:
@@ -432,7 +566,7 @@
 /* warning : do not handle zones larger than one block */
 static int getFileContent(mx_device *pDev, u_char *buf, int inode_num)
 {
    u_long nbread = 0, remaining, filesize, zone_num, i;
     mx_inode_info inode;
  
     inode.inode_num = inode_num;

             break;
     zone_num = inode.data_zones[i];
     remaining = filesize - nbread;
    if (remaining > pDev->super->s_blocksize)
        remaining = pDev->super->s_blocksize;
    if (readBlock(pDev, pDev->fd, zone_num, buf+nbread, remaining)) {
             resetInode(&inode);
             snprintf(errormsg, sizeof(errormsg), 
                 "error in reading file content");
@@ -636,6 +770,7 @@
     i += 16;
     printf("\n");
     }
    
     free(buf);
     resetInode(&inode);
 }

     mx_inode_info inode;
     char mode_string[8];
     char *extractPath = NULL;
    
    fprintf(pFile, "INODE: %d \n", inode_num);
  
     inode.inode_num = inode_num;
     if (initInode(pDev, &inode) != 0)

         int i, child_inode, entrylen = 16;
         u_short us;
  
        fprintf(pFile, "dir: %s %s %d %d\n", path, mode_string, 
             inode.uid, inode.gid);
         /* create local dir here */
         if (extractDir != NULL) {

             goto fail;
         }
        filecontent[inode.size] = 0;
        fprintf(pFile, "soft l.: %s %s %d %d %s\n", path, mode_string, inode.uid, 
             inode.gid, filecontent);
  
     }

         char *relpath;
         unsigned char *filecontent;
         int filesize, nbwritten, fd;
         relpath = path;
         while (relpath[0] != '\0' && relpath[0] == '/') /* skip leading "/" */
             relpath++;
         if (relpath[0] == '\0')
             goto fail;
        fprintf(pFile, "file: %s %s %d %d %s\n", path, mode_string, 
             inode.uid & MINIX_UID_MASK, inode.gid, relpath);
         if (extractDir != NULL) {
             filesize = inode.size;

                 if (getFileContent(pDev, filecontent, inode.inode_num)) {
                     free(filecontent);
                     goto fail;

                }printf("Size: %d\n", filesize);
                 if (inode.compressed) {
                     unsigned char *uncompressed;
  

         /* file size is null, buf first zone num is used for major and minor */
         major = inode.raw_zone[0] >> 8;
         minor = inode.raw_zone[0] & 0xFF;
        fprintf(pFile, "block. dev.: %s %s %d %d %d %d\n", path, mode_string, inode.uid, 
             inode.gid, major, minor);
     }
     resetInode(&inode);

         "%d %d\n"
         "# root dir perm\n", pDev->super->nb_zone, pDev->super->nb_inode);
     if (printProtoLine(pFile, "", pDev, 1, extractDir) != 0)
       return -1;  
     fprintf(pFile, "$\n");
     return 0;
}

  
         memset(inbuf, 0, cluster_size);
         memset(outbuf, 0, cluster_size);
        if (tab[cluster_idx+1] < tab[cluster_idx] ){/* ???????????????????????????????????????? */
            memcpy(out,in,filesize);
            printf("Error in decompression.");
            return 0;
        }
         memcpy(inbuf, in + tab[cluster_idx], 
             tab[cluster_idx+1] - tab[cluster_idx]);
         if (minix_uncompress_cluster(outbuf, cluster_size, inbuf,
