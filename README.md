# Shape modeling
## Assignment 6

### Resources 

- [Our Presentation](https://docs.google.com/presentation/d/1DIcviOEhOQk6m5dT0ryPQ2L86IimTuuQazykyQr2f8I/edit?usp=sharing)

- [Our report](report.md)

- [Our meshes (polybox)](https://polybox.ethz.ch/index.php/s/TjBI4GNID1qZCtm?path=%2F)

- [Files, shared with all teams (polybox)](https://polybox.ethz.ch/index.php/s/ZfYXXfV5SR4sQoB)  (PW: eigenfaces)

### Group Members

| Legi Number | First Name | Last Name       | Email            | Github Username |
| ----------- | ---------- | ----------------| ---------------- | --------------- |
| 16-944-530  | Viviane    | Yang            | vyang@ethz.ch    | vyangETH        |
| 17-948-191  | Daniel     | Sparber         | dsparber@ethz.ch | DanielSparber   |
| 15-941-222  | Sebastian  | Winberg         | winbergs@ethz.ch | winbergs        |
| 18-949-685  | Camilla    | Casamento Tumeo | camillca@ethz.ch | ccasam          |
| 15-923-428  | Jela       | Kovacevic       | jelak@ethz.ch    | jelak           |

### Work distribution

- Camilla: Preprocessing, Face Alignment (Rigid)
- Daniel: Project Lead, Learning Based Approach (bonus)
- Jela: Face Alignment (Warping)
- Sebastian: Landmark Selection, Own Scans, User Interface
- Viviane: PCA

### Collaborations

- The manual work of selecting landmarks was distributed among the groups
- Smooth the mesh boundaries
- Keep 22 landmarks (no cheeks, to avoid ambiguity)
- Shared own scans and landmarks with other teams

### Group Meetings

- 09.05.2020, 14:00
- 20.05.2020, 11:20
- 26.05.2020, 10:00
- 28.05.2020, 10:00
- 28.05.2020, 17:30

### Note about User Interface

In case the User Interface by ImGui look off and/or are partially scaled incorrectly, please change the two variables `SCREEN_SCALE_X` and `SCREEN_SCALE_Y` in the file `common.h`.

### Project Structure

- `src/main.cpp` is the entry point for an application containing all pipline steps

- `test/` contains main files for every pipline step individually

- `learning/` contains the learning based approach (PyTorch)


### How To

#### Cloning the repository

```
git clone --recursive https://github.com/DanielSparber/sm-assignment6.git
```

#### Building

```
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j2
```

#### Add a new step to the pipeline

1. Add header and source file in `src`
2. Write your own test file in `test`
3. Add an executable for your step in CMakeLists.txt
4. After testing, add your files and functions in the pipeline main.cpp

