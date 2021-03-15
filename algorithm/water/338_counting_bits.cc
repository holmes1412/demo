// https://leetcode-cn.com/problems/counting-bits/
// return an array of counting binary bits 1 from 0 to num
// 16 -> [0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,1]

class Solution {
public:
    vector<int> countBits(int num) {
        vector<int> a;
        a.reserve(num + 1);
        a.push_back(0);
        unsigned int flag = 0;
        for (int i = 1; i <= num; i++)
        {
            if (i == (1<<flag))
            {
                flag++;
                a.push_back(1);
            }
            else
            {
                a.push_back(a[i - (1<<(flag - 1))] + 1);
            }
        }
        return a;
    }
};
