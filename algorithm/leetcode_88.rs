// leetcode 88: merge two sorted array into the first one

pub struct Solution {
}

impl Solution {
    pub fn merge(nums1: &mut Vec<i32>, m: i32, nums2: &mut Vec<i32>, n: i32) {
        if n == 0 {
            return;
        }
        
        if m == 0 {
            for i in 0..n as usize {
                nums1[i] = nums2[i];
            }
            return;
        }

        let mut i = m - 1;
        let mut j = n - 1;
        let mut l = m + n - 1;

        while l >= 0 && i >= 0 && j >= 0 {
            if nums1[i as usize] > nums2[j as usize] {
                nums1[l as usize] = nums1[i as usize];
                i = i - 1;
            } else {
                nums1[l as usize] = nums2[j as usize];
                j = j - 1;
            }
            l = l - 1;
        }
        while j >= 0 {
            nums1[l as usize] = nums2[j as usize];
            j = j - 1;
            l = l - 1;
        }
    }
}
