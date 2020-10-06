mod leetcode_27;
use leetcode_27::Solution;

fn main() {
    let mut arr : std::vec::Vec<i32> = [0,1,2,2,3,0,4,2].to_vec();
    let ret = Solution::remove_element(&mut arr, 2);
    Solution::print(&mut arr, ret);
}
