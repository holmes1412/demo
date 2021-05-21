class H2O {
public:
    H2O() {
        this->h_count = 0;
        this->o_count = 0;
    }   

    void hydrogen(function<void()> releaseHydrogen) {
        pthread_mutex_lock(&mutex);
        while (this->o_count != 1)
            pthread_cond_wait(&h_cond, &mutex);
        pthread_mutex_unlock(&mutex);          
        this->h_count++;
        // releaseHydrogen() outputs "H". Do not change or remove this line.
        releaseHydrogen();
        if (this->h_count == 2)
        {   
            this->h_count = 0;
            this->o_count = 0;
            pthread_cond_signal(&o_cond);
        }
    }

    void oxygen(function<void()> releaseOxygen) {
        pthread_mutex_lock(&mutex);
        while (this->o_count != 0)
            pthread_cond_wait(&o_cond, &mutex);
        pthread_mutex_unlock(&mutex);
        this->o_count = 1;
        // releaseOxygen() outputs "O". Do not change or remove this line.
        releaseOxygen();
        pthread_cond_signal(&h_cond);
    }   

private:
    int h_count;
    int o_count;
    pthread_cond_t h_cond = PTHREAD_COND_INITIALIZER;
    pthread_cond_t o_cond = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
};
