//mod leetcode_27;
//use leetcode_27::Solution;
mod leetcode_876;

fn main() {
//    println!("Hello, world!");
//    let s = Solution::new();
//    let mut arr : std::vec::Vec<i32> = [0,1,2,2,3,0,4,2].to_vec();
//    let ret = leetcode_27::Solution::remove_element(&mut arr, 2);
//    leetcode_27::Solution::print(&mut arr, ret);

    let mut test = std::vec::Vec::new();
    test.push([1, 2, 3].to_vec());
    test.push([4, 5, 6].to_vec());
    let ret = leetcode_876::Solution::transpose(test);
    leetcode_876::Solution::print(ret);
}
