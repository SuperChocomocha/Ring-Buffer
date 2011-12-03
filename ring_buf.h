/*------------------------------------------------------------------------------
 * Max Guo
 * December 2, 2011
 * Ring Buffer implementation
 *----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 * USAGE: - Ring Buffer header file, can be used wherever, whenever, however
 *----------------------------------------------------------------------------*/

typedef struct {
    char *buf_ptr;
    int buf_size;
    int free_slots;
    int read_ptr;
    int write_ptr;
} ring_buffer;

/*------------------------------------------------------------------------------
 * create_buf() method - initializes circular buffer to a certain size
 * inputs:
 *     int size - size of circular buffer
 * returns:
 *     ring_buffer* - pointer to created circular buffer
 *----------------------------------------------------------------------------*/
ring_buffer *create_buf(int size) {
    ring_buffer *buf = (ring_buffer *)malloc(sizeof(ring_buffer));
    buf->buf_ptr = (char *)calloc(size, sizeof(char));
    buf->buf_size = size;
    buf->free_slots = size;
    buf->read_ptr = 0;
    buf->write_ptr = 0;
    return buf;
}

/*------------------------------------------------------------------------------
 * get_max_read_size() method - returns how much data can be read
 * inputs:
 *     ring_buffer *buf - buffer to check
 * returns:
 *     int - maximum size of data that can be read
 *----------------------------------------------------------------------------*/
int get_max_read_size(ring_buffer *buf) {
    if (buf->buf_ptr) {
        if (buf->read_ptr == buf->write_ptr) {
            if (buf->free_slots == buf->buf_size) {
                return 0;
            } else {
                return buf->buf_size;
            }
        }
        if (buf->read_ptr < buf->write_ptr) {
            return buf->write_ptr - buf->read_ptr;
        }
        if (buf->read_ptr > buf->write_ptr) {
            return buf->buf_size - buf->read_ptr + buf->write_ptr;
        }
    }
    return 0;
}

/*------------------------------------------------------------------------------
 * get_max_write_size() method - returns how much space can be written to
 * inputs:
 *     ring_buffer *buf - buffer to check
 * returns:
 *     int - maximum amount of space that can be written to
 *----------------------------------------------------------------------------*/
int get_max_write_size(ring_buffer *buf) {
    if (buf->buf_ptr) {
        if (buf->read_ptr == buf->write_ptr) {
            if (buf->free_slots == buf->buf_size) {
                return buf->buf_size;
            } else {
                return 0;
            }
        }
        if (buf->write_ptr < buf->read_ptr) {
            return buf->read_ptr - buf->write_ptr;
        }
        if (buf->write_ptr > buf->read_ptr) {
            return buf->buf_size - buf->write_ptr + buf->read_ptr;
        }
    }
    return 0;
}

/*------------------------------------------------------------------------------
 * write_to_buf() method - writes data to circular buffer, does not overwrite
 * inputs:
 *     ring_buffer *buf - buffer to write to
 *     char *data - data to write
 *     int length - length of data
 * returns:
 *     void - no return value
 *----------------------------------------------------------------------------*/
void write_to_buf(ring_buffer *buf, char *data, int length) {
    if (length <= get_max_write_size(buf)) {
        if (buf->write_ptr + length < buf->buf_size) {       //no wrap-around
            memcpy(&buf->buf_ptr[buf->write_ptr], data, length);
            buf->write_ptr += length;
        } else {                                             //data wraps around
            int first_size = buf->buf_size - buf->write_ptr;
            int second_size = length - first_size;
            memcpy(&buf->buf_ptr[buf->write_ptr], data, first_size);
            memcpy(&buf->buf_ptr[0], &data[first_size], second_size);
            buf->write_ptr = second_size;
        }
    }
}

/*------------------------------------------------------------------------------
 * write_to_buf_over() method - writes data to circular buffer, overwrites
 * inputs:
 *     ring_buffer *buf - buffer to write to
 *     char *data - data to write
 *     int length - length of data
 * returns:
 *     void - no return value
 *----------------------------------------------------------------------------*/
void write_to_buf_over(ring_buffer *buf, char *data, int length) {
    if (buf->write_ptr + length < buf->buf_size) {           //no wrap-around
        memcpy(&buf->buf_ptr[buf->write_ptr], data, length);
        buf->write_ptr += length;
    } else {                                                 //data wraps around
        int first_size = buf->buf_size - buf->write_ptr;
        int second_size = length - first_size;
        memcpy(&buf->buf_ptr[buf->write_ptr], data, first_size);
        memcpy(&buf->buf_ptr[0], &data[first_size], second_size);
        buf->write_ptr = second_size;
    }
    if (buf->free_slots == 0) {
        buf->read_ptr = buf->write_ptr;
    }
    if (length <= get_max_write_size(buf)) {
        buf->free_slots -= length;
    }
}

/*------------------------------------------------------------------------------
 * read_buf() method - read data from circular buffer
 * inputs:
 *     ring_buffer *buf - buffer to read from
 *     char *store - read data into this storage
 *     int length - length of data
 * returns:
 *     int - 0 on failed read
 *           1 on successful read
 *----------------------------------------------------------------------------*/
int read_buf(ring_buffer *buf, char *store, int length) {
    int r = 0;
    if (length <= get_max_read_size(buf)) {
        if (buf->read_ptr + length < buf->buf_size) {        //no wrap-around
            memcpy(store, &buf->buf_ptr[buf->read_ptr], length);
            buf->read_ptr += length;
        } else {                                             //data wraps around
            int first_size = buf->buf_size - buf->read_ptr;
            int second_size = length - first_size;
            memcpy(store, &buf->buf_ptr[buf->read_ptr], first_size);
            memcpy(&store[first_size], &buf->buf_ptr[0], second_size);
            buf->read_ptr = second_size;
        }
        r = 1;
    }
    return r;
}

/*------------------------------------------------------------------------------
 * free_buf() method - cleanup method, frees buffer memory
 * inputs:
 *     ring_buffer *buf - buffer to cleanup
 * returns:
 *     void - no return value
 *----------------------------------------------------------------------------*/
void free_buf(ring_buffer *buf) {
    if(buf->buf_ptr) {
        free(buf->buf_ptr);
    }
    buf->buf_ptr = NULL;
    buf->buf_size = 0;
    buf->read_ptr = 0;
    buf->write_ptr = 0;
}
