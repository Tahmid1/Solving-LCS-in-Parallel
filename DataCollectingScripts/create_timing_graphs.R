
rm(list=ls())
library(ggplot2)
seq_tab_data <- read.csv("seq_tab_data.txt")
ggplot(seq_tab_data,aes(x=SEQUENCE_LENGTH,y=SECS)) + geom_point(size=3) + 
  stat_smooth(method="lm", se=FALSE, formula=y ~ poly(x, 2, raw=TRUE), color="blue") + ggtitle("Sequential Tabulation")

seq_mem_data <- read.csv("seq_mem_data.txt")
ggplot(seq_mem_data,aes(x=SEQUENCE_LENGTH,y=SECS)) + geom_point(size=3) + 
  stat_smooth(method="lm", se=FALSE, formula=y ~ poly(x, 2, raw=TRUE), color="orange") + ggtitle("Sequential Memoization")

