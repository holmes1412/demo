class Solution {
public:
    void searchSolutions(vector<int>& candidates, int target, int depth, vector<int>& path, vector<vector<int>>& solutions) {
        if (depth >= candidates.size()) {
            return;
        }
        int count = 0;
        while (candidates[depth] * count <= target) {
            int cur_target = target - candidates[depth] * count;
            path[depth] = count;
            if (cur_target == 0) {
                vector<int> solution;
                for (int i = 0; i <= depth; i ++) {
                    for (int j = 0; j < path[i]; j ++) {
                        solution.push_back(candidates[i]);
                    }
                }
                solutions.push_back(solution);
                return;
            }
            searchSolutions(candidates, cur_target, depth + 1, path, solutions);
            count ++;
        }
        path[depth] = 0;
    }

    vector<vector<int>> combinationSum(vector<int>& candidates, int target) {
        vector<vector<int>> solutions;
        vector<int> path;
        path.resize(candidates.size());
        searchSolutions(candidates, target, /* depth = */ 0, path, solutions);
        return solutions;
    }
};
