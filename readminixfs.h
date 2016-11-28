--- orig-readminix/readminixfs.h    2004-02-23 15:00:00.000000000 +0100
+++ readminixfs.h    2013-11-26 21:12:20.000000000 +0100
@@ -30,8 +30,13 @@
 #define MINIX2_SUPER_MAGIC2_REV     0x7824
 #define MINIX2_SUPER_MAGIC2_COMPR_REV  0x8824
  
-#define INODE_PER_BLOCK_V1 32
-#define INODE_PER_BLOCK_V2 16
+#define MINIX25_SUPER_MAGIC2      0x38D1 /* minix V2.5 fs, 30 char names */
+#define MINIX25_SUPER_MAGIC2_COMPR 0x38E1 /* minix V2.5 fs, 30 char names, comp */
+#define MINIX25_SUPER_MAGIC2_REV      0xD138 /* minix V2.5 fs, 30 char names */
+#define MINIX25_SUPER_MAGIC2_COMPR_REV 0xE138 /* minix V2.5 fs, 30 char names, comp */
+
+/*#define INODE_PER_BLOCK_V1 32
+#define INODE_PER_BLOCK_V2 16 */
  
 #define ROOT_INODE 1
 #define MINIX_HEADER_SIZE 7
@@ -49,6 +54,22 @@
     u_short magic;
     u_short clean;
     u_long nb_zone_v2;
+    u_short s_magic_v3;       /* only used on v3 */
+    u_short s_pad2;           /* only used on v3 */
+    u_short s_blocksize;      /* used on v2.5 and v3 */
+    u_char  s_disk_version;   /* only used on v3 */
+    u_char  s_res0;
+    /* 0x20 */
+    u_char  s_pad[0x1C0 - 0x020];
+    /* 0x1C0 */
+    /* Additional structure, for Netgem extensions */
+    u_char ng_uuid[16];  /* 128 bits UUID */
+    u_char  ng_label[16]; /* volume label  */
+    /* 0x1E0 */
+    u_char  ng_pad0[28];
+#define MINIX_NG_FLAG_RO 0x0001
+    u_char ng_flags;
+    u_char ng_magic; 
 } mx_sb_disk;
  
 typedef struct {
@@ -61,6 +82,7 @@
     int max_file_size;
     int magic;
     int clean;
+    int s_blocksize;
  
     /* fieds only in memory */
     u_char *inode_map;
@@ -87,6 +109,25 @@
     u_short zone[9];
 } mx_inode_disk; /* size : 32 bytes */
  
+/* exact mapping of minix V2 inode */
+/*
+ * The new minix inode has all the time entries, as well as
+ * long block numbers and a third indirect block (7+1+1+1
+ * instead of 7+1+1). Also, some previously 8-bit values are
+ * now 16-bit. The inode is now 64 bytes instead of 32.
+ */
+typedef struct {
+    u_short mode;
+    u_short nlinks;
+    u_short uid;
+    u_short gid;
+    u_long size;
+    u_long atime;
+    u_long mtime;
+    u_long ctime;
+    u_long zone[10];
+} mx2_inode_disk; 
+
 typedef struct {
     int inode_num;
     int mode;
@@ -95,11 +136,11 @@
     int size;
     int time;
     int nb_link;
-    int raw_zone[9];
-    int nb_data_zone;
-    int nb_indirect_zone;
-    int *data_zones;
-    int *indirect_zones; /* 8th zone, 9th zone and 9th content */
+    u_long raw_zone[10];
+    u_long nb_data_zone;
+    u_long nb_indirect_zone;
+    u_long *data_zones;
+    u_long *indirect_zones; /* 8th zone, 9th zone and 9th content */
     char compressed; /* 0 not compressed, 1 compressed */
 } mx_inode_info;
