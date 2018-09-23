/* Generated by scripts/elf32_to_elf64.pl */
/* DO NOT EDIT !!! */

/************************************************************************/
/* rop-tool - A Return Oriented Programming and binary exploitation     */
/*            tool                                                      */
/*                                                                      */
/* Copyright 2013-2015, -TOSH-                                          */
/* File coded by -TOSH-                                                 */
/*                                                                      */
/* This file is part of rop-tool.                                       */
/*                                                                      */
/* rop-tool is free software: you can redistribute it and/or modify     */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or    */
/* (at your option) any later version.                                  */
/*                                                                      */
/* rop-tool is distributed in the hope that it will be useful,          */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        */
/* GNU General Public License for more details.                         */
/*                                                                      */
/* You should have received a copy of the GNU General Public License    */
/* along with rop-tool.  If not, see <http://www.gnu.org/licenses/>     */
/************************************************************************/
#include "binfmt.h"
#include "binfmt/elf.h"

/* =========================================================================
   This file implement functions for parsing ELF64 binaries
   ======================================================================= */


r_binfmt_ssp_e r_binfmt_elf_check_ssp(r_binfmt_s *bin);
r_binfmt_type_e r_binfmt_elf_type(r_binfmt_s *bin);

/* Internal ELF structure */
typedef struct {
  Elf64_Ehdr *ehdr;

  Elf64_Shdr *shdr;
  Elf64_Half shdr_entries;

  Elf64_Phdr *phdr;
  Elf64_Half phdr_entries;

  Elf64_Sym *symtab;
  Elf64_Word symtab_entries;

  Elf64_Sym *dynsym;
  Elf64_Word dynsym_entries;

  Elf64_Dyn *dyntab;
  Elf64_Word dyntab_entries;
}r_binfmt_elf64_s;


void r_binfmt_elf64_load_ehdr(r_binfmt_s *bin, r_binfmt_elf64_s *elf) {

  if(bin->mapped_size < sizeof(Elf64_Ehdr)) {
    return;
  }

  elf->ehdr = (Elf64_Ehdr*)(bin->mapped);
}

void r_binfmt_elf64_load_phdr(r_binfmt_s *bin, r_binfmt_elf64_s *elf) {
  Elf64_Off e_phoff;
  Elf64_Half e_phnum;

  R_BINFMT_ASSERT(elf->ehdr != NULL);

  R_BINFMT_GET_INT(e_phoff, elf->ehdr->e_phoff, bin->endian);
  R_BINFMT_ASSERT(e_phoff < bin->mapped_size);

  R_BINFMT_GET_INT(e_phnum, elf->ehdr->e_phnum, bin->endian);

  R_BINFMT_ASSERT(r_utils_add64(NULL, e_phnum*sizeof(Elf64_Phdr), e_phoff) &&
                  e_phnum*sizeof(Elf64_Phdr) + e_phoff <= bin->mapped_size);

  elf->phdr = (Elf64_Phdr*)(bin->mapped + e_phoff);
  elf->phdr_entries = e_phnum;
}

void r_binfmt_elf64_load_shdr(r_binfmt_s *bin, r_binfmt_elf64_s *elf) {
  Elf64_Off e_shoff;
  Elf64_Half e_shnum;

  R_BINFMT_ASSERT(elf->ehdr != NULL);

  R_BINFMT_GET_INT(e_shoff, elf->ehdr->e_shoff, bin->endian);
  R_BINFMT_ASSERT(e_shoff < bin->mapped_size);

  R_BINFMT_GET_INT(e_shnum, elf->ehdr->e_shnum, bin->endian);

  R_BINFMT_ASSERT(r_utils_add64(NULL, e_shnum*sizeof(Elf64_Shdr), e_shoff) &&
                  e_shnum*sizeof(Elf64_Shdr) + e_shoff <= bin->mapped_size);

  elf->shdr = (Elf64_Shdr*)(bin->mapped + e_shoff);
  elf->shdr_entries = e_shnum;
}

void r_binfmt_elf64_load_symtab(r_binfmt_s *bin, r_binfmt_elf64_s *elf) {
  Elf64_Word i, sh_type, sh_size;
  Elf64_Off sh_offset;

  R_BINFMT_ASSERT(elf->ehdr != NULL);
  R_BINFMT_ASSERT(elf->shdr != NULL);


  for(i = 0; i < elf->shdr_entries; i++) {
    R_BINFMT_GET_INT(sh_type, elf->shdr[i].sh_type, bin->endian);

    if(sh_type == SHT_SYMTAB) {
      R_BINFMT_GET_INT(sh_size, elf->shdr[i].sh_size, bin->endian);

      R_BINFMT_ASSERT((sh_size % sizeof(Elf64_Sym)) == 0);

      R_BINFMT_GET_INT(sh_offset, elf->shdr[i].sh_offset, bin->endian);

      R_BINFMT_ASSERT(r_utils_add64(NULL, sh_offset, sh_size) &&
                      sh_offset + sh_size <= bin->mapped_size);

      elf->symtab = (Elf64_Sym*)(bin->mapped + sh_offset);
      elf->symtab_entries = sh_size / sizeof(Elf64_Sym);
      break;
    }
  }
}

void r_binfmt_elf64_load_dynsym(r_binfmt_s *bin, r_binfmt_elf64_s *elf) {
  Elf64_Word i, sh_type, sh_size;
  Elf64_Off sh_offset;

  R_BINFMT_ASSERT(elf->ehdr != NULL);
  R_BINFMT_ASSERT(elf->shdr != NULL);


  for(i = 0; i < elf->shdr_entries; i++) {
    R_BINFMT_GET_INT(sh_type, elf->shdr[i].sh_type, bin->endian);

    if(sh_type == SHT_DYNSYM) {
      R_BINFMT_GET_INT(sh_size, elf->shdr[i].sh_size, bin->endian);

      R_BINFMT_ASSERT((sh_size % sizeof(Elf64_Sym)) == 0);

      R_BINFMT_GET_INT(sh_offset, elf->shdr[i].sh_offset, bin->endian);

      R_BINFMT_ASSERT(r_utils_add64(NULL, sh_offset, sh_size) &&
                      sh_offset + sh_size <= bin->mapped_size);

      elf->dynsym = (Elf64_Sym*)(bin->mapped + sh_offset);
      elf->dynsym_entries = sh_size / sizeof(Elf64_Sym);
      break;
    }
  }
}


void r_binfmt_elf64_load_dyntab(r_binfmt_s *bin, r_binfmt_elf64_s *elf) {
  Elf64_Word i, sh_type, sh_size;
  Elf64_Off sh_offset;

  R_BINFMT_ASSERT(elf->ehdr != NULL);
  R_BINFMT_ASSERT(elf->shdr != NULL);

  for(i = 0; i < elf->shdr_entries; i++) {
    R_BINFMT_GET_INT(sh_type, elf->shdr[i].sh_type, bin->endian);

    if(sh_type == SHT_DYNAMIC) {
      R_BINFMT_GET_INT(sh_size, elf->shdr[i].sh_size, bin->endian);

      R_BINFMT_ASSERT((sh_size % sizeof(Elf64_Dyn)) == 0);

      R_BINFMT_GET_INT(sh_offset, elf->shdr[i].sh_offset, bin->endian);

      R_BINFMT_ASSERT(r_utils_add64(NULL, sh_offset, sh_size) &&
                      sh_offset + sh_size <= bin->mapped_size);

      elf->dyntab = (Elf64_Dyn*)(bin->mapped + sh_offset);
      elf->dyntab_entries = sh_size / sizeof(Elf64_Dyn);
      break;
    }
  }
}


/* Fill bin->segments structure */
static void r_binfmt_elf64_load_segments(r_binfmt_s *bin, r_binfmt_elf64_s *elf) {
  r_binfmt_segment_s *seg;
  Elf64_Word i, p_type, p_flags, p_filesz;
  Elf64_Off p_offset;
  Elf64_Addr p_vaddr;
  Elf64_Half e_phnum;

  R_BINFMT_ASSERT(elf->ehdr != NULL);
  R_BINFMT_ASSERT(elf->phdr != NULL);

  R_BINFMT_GET_INT(e_phnum, elf->ehdr->e_phnum, bin->endian);

  for(i = 0; i < e_phnum; i++) {
    R_BINFMT_GET_INT(p_type, elf->phdr[i].p_type, bin->endian);
    R_BINFMT_GET_INT(p_flags, elf->phdr[i].p_flags, bin->endian);
    R_BINFMT_GET_INT(p_vaddr, elf->phdr[i].p_vaddr, bin->endian);
    R_BINFMT_GET_INT(p_offset, elf->phdr[i].p_offset, bin->endian);
    R_BINFMT_GET_INT(p_filesz, elf->phdr[i].p_filesz, bin->endian);

    R_BINFMT_ASSERT(r_utils_add64(NULL, p_offset, p_filesz) &&
                    p_offset + p_filesz <= bin->mapped_size);

    if(p_type == PT_LOAD) {
      seg = r_binfmt_segment_new();

      seg->flags = 0;
      if(p_flags & PF_X)
        seg->flags |= R_BINFMT_SEGMENT_FLAG_PROT_X;
      if(p_flags & PF_R)
        seg->flags |= R_BINFMT_SEGMENT_FLAG_PROT_R;
      if(p_flags & PF_W)
        seg->flags |= R_BINFMT_SEGMENT_FLAG_PROT_W;

      seg->addr = p_vaddr;
      seg->length = p_filesz;
      seg->start = bin->mapped + p_offset;

      r_utils_linklist_push(&bin->segments, seg);
    }
  }
}

static const char* r_binfmt_elf64_get_name(r_binfmt_s *bin, r_binfmt_elf64_s *elf, Elf64_Half section_id, Elf64_Word name) {
  Elf64_Off sh_offset;
  Elf64_Word sh_size;

  R_BINFMT_GET_INT(sh_offset, elf->shdr[section_id].sh_offset, bin->endian);
  R_BINFMT_GET_INT(sh_size, elf->shdr[section_id].sh_size, bin->endian);

  R_BINFMT_ASSERT_RET("", r_utils_add64(NULL, sh_offset, sh_size) &&
                      sh_offset + sh_size <= bin->mapped_size);

  R_BINFMT_ASSERT_RET("", name < sh_size);

  return (const char*)(bin->mapped + sh_offset + name);
}

static void r_binfmt_elf64_load_syms_symtab(r_binfmt_s *bin, r_binfmt_elf64_s *elf) {
  r_binfmt_sym_s *sym;
  Elf64_Half sh_link, st_name;
  Elf64_Off link_off;
  Elf64_Word i, sh_type;

  R_BINFMT_ASSERT(elf->ehdr != NULL);
  R_BINFMT_ASSERT(elf->shdr != NULL);
  R_BINFMT_ASSERT(elf->symtab != NULL);

  sh_link = 0xFFFF;
  for(i = 0; i < elf->shdr_entries; i++) {
    R_BINFMT_GET_INT(sh_type, elf->shdr[i].sh_type, bin->endian);

    if(sh_type == SHT_SYMTAB) {
      R_BINFMT_GET_INT(sh_link, elf->shdr[i].sh_link, bin->endian);
      R_BINFMT_ASSERT(sh_link < elf->shdr_entries);
      break;
    }
  }

  if(sh_link == 0xFFFF)
    return;

  for(i = 0; i < elf->symtab_entries; i++) {
    R_BINFMT_GET_INT(st_name, elf->symtab[i].st_name, bin->endian);
    R_BINFMT_GET_INT(link_off, elf->shdr[sh_link].sh_offset, bin->endian);

    R_BINFMT_ASSERT(r_utils_add64(NULL, link_off, st_name) &&
                    link_off + st_name <= bin->mapped_size);

    sym = r_binfmt_sym_new();
    sym->name = r_binfmt_elf64_get_name(bin, elf, sh_link, st_name);
    sym->addr = elf->symtab[i].st_value;
    r_utils_arraylist_push(&bin->syms, sym);
  }
}


static void r_binfmt_elf64_load_syms_dyntab(r_binfmt_s *bin, r_binfmt_elf64_s *elf) {
  r_binfmt_sym_s *sym;
  Elf64_Half sh_link, st_name;
  Elf64_Off link_off;
  Elf64_Word i, sh_type;

  R_BINFMT_ASSERT(elf->ehdr != NULL);
  R_BINFMT_ASSERT(elf->shdr != NULL);
  R_BINFMT_ASSERT(elf->dynsym != NULL);

  sh_link = 0xFFFF;
  for(i = 0; i < elf->shdr_entries; i++) {
    R_BINFMT_GET_INT(sh_type, elf->shdr[i].sh_type, bin->endian);

    if(sh_type == SHT_DYNSYM) {
      R_BINFMT_GET_INT(sh_link, elf->shdr[i].sh_link, bin->endian);
      R_BINFMT_ASSERT(sh_link < elf->shdr_entries);
      break;
    }
  }

  if(sh_link == 0xFFFF)
    return;

  for(i = 0; i < elf->dynsym_entries; i++) {
    R_BINFMT_GET_INT(st_name, elf->dynsym[i].st_name, bin->endian);
    R_BINFMT_GET_INT(link_off, elf->shdr[sh_link].sh_offset, bin->endian);

    R_BINFMT_ASSERT(r_utils_add64(NULL, link_off, st_name) &&
                    link_off + st_name <= bin->mapped_size);

    sym = r_binfmt_sym_new();
    sym->name = r_binfmt_elf64_get_name(bin, elf, sh_link, st_name);
    sym->addr = elf->dynsym[i].st_value;
    r_utils_arraylist_push(&bin->syms, sym);
  }
}

static void r_binfmt_elf64_load_syms(r_binfmt_s *bin, r_binfmt_elf64_s *elf) {
  if(elf->dyntab != NULL)
    r_binfmt_elf64_load_syms_dyntab(bin, elf);
  if(elf->symtab != NULL)
    r_binfmt_elf64_load_syms_symtab(bin, elf);
}

/* Fill bin->sections structure */
static void r_binfmt_elf64_load_sections(r_binfmt_s *bin, r_binfmt_elf64_s *elf) {
  r_binfmt_section_s *section;
  Elf64_Word sh_name, sh_size, i;
  Elf64_Off strndx_off;
  Elf64_Addr sh_addr;
  Elf64_Half e_shstrndx;

  R_BINFMT_ASSERT(elf->ehdr != NULL);
  R_BINFMT_ASSERT(elf->shdr != NULL);

  R_BINFMT_GET_INT(e_shstrndx, elf->ehdr->e_shstrndx, bin->endian);
  R_BINFMT_ASSERT(e_shstrndx < elf->shdr_entries);

  R_BINFMT_GET_INT(strndx_off, elf->shdr[e_shstrndx].sh_offset, bin->endian);

  for(i = 0; i < elf->shdr_entries; i++) {
    R_BINFMT_GET_INT(sh_addr, elf->shdr[i].sh_addr, bin->endian);
    R_BINFMT_GET_INT(sh_size, elf->shdr[i].sh_size, bin->endian);
    R_BINFMT_GET_INT(sh_name, elf->shdr[i].sh_name, bin->endian);

    R_BINFMT_ASSERT(r_utils_add64(NULL, strndx_off, sh_name) &&
                    strndx_off + sh_name <= bin->mapped_size);

    section = r_binfmt_section_new();
    section->addr = sh_addr;
    section->size = sh_size;
    section->name = r_binfmt_elf64_get_name(bin, elf, e_shstrndx, sh_name);

    r_utils_linklist_push(&bin->sections, section);
  }
}

/* Get the architecture */
static r_binfmt_arch_e r_binfmt_elf64_getarch(r_binfmt_s *bin, r_binfmt_elf64_s *elf) {
  uint16_t e_machine;

  R_BINFMT_ASSERT_RET(R_BINFMT_ARCH_UNDEF, elf->ehdr != NULL);
  R_BINFMT_GET_INT(e_machine, elf->ehdr->e_machine, bin->endian);

  if(e_machine == EM_386)
    return R_BINFMT_ARCH_X86;
  if(e_machine == EM_ARM)
    return R_BINFMT_ARCH_ARM;
  if(e_machine == EM_X86_64 ||
     e_machine == EM_IA_64)
    return R_BINFMT_ARCH_X86_64;
  if(e_machine == EM_AARCH64)
    return R_BINFMT_ARCH_ARM64;
  if(e_machine == EM_MIPS)
    return R_BINFMT_ARCH_MIPS64;

  return R_BINFMT_ARCH_UNDEF;
}

/* Get the endianness */
static r_binfmt_endian_e r_binfmt_elf64_getendian(r_binfmt_s *bin) {
  R_BINFMT_ASSERT_RET(R_BINFMT_ENDIAN_UNDEF, bin->mapped_size >= sizeof(Elf64_Ehdr));

  if(bin->mapped[EI_DATA] == ELFDATA2LSB)
    return R_BINFMT_ENDIAN_LITTLE;
  if(bin->mapped[EI_DATA] == ELFDATA2MSB)
    return R_BINFMT_ENDIAN_BIG;

  return R_BINFMT_ENDIAN_UNDEF;
}

/* Get entry point */
static addr_t r_binfmt_elf64_getentry(r_binfmt_s *bin, r_binfmt_elf64_s *elf) {
  Elf64_Addr e_entry;

  R_BINFMT_ASSERT_RET(0, elf->ehdr != NULL);

  R_BINFMT_GET_INT(e_entry, elf->ehdr->e_entry, bin->endian);
  return e_entry;
}

Elf64_Phdr* r_binfmt_elf64_get_segment(r_binfmt_s *bin, r_binfmt_elf64_s *elf, Elf64_Word type, Elf64_Word flags) {
  Elf64_Word i, p_type, p_flags;

  R_BINFMT_ASSERT_RET(NULL, elf->ehdr != NULL);
  R_BINFMT_ASSERT_RET(NULL, elf->phdr != NULL);

  for(i = 0; i < elf->phdr_entries; i++) {
    R_BINFMT_GET_INT(p_type, elf->phdr[i].p_type, bin->endian);
    R_BINFMT_GET_INT(p_flags, elf->phdr[i].p_flags, bin->endian);
    if(p_type == type && p_flags & flags)
      return &elf->phdr[i];
  }
  return NULL;
}

/* Check if NX bit is enabled */
static r_binfmt_nx_e r_binfmt_elf64_check_nx(r_binfmt_s *bin, r_binfmt_elf64_s *elf) {
  R_BINFMT_ASSERT_RET(R_BINFMT_NX_UNKNOWN, elf->ehdr != NULL);
  R_BINFMT_ASSERT_RET(R_BINFMT_NX_UNKNOWN, elf->phdr != NULL);

  if(r_binfmt_elf64_get_segment(bin, elf, PT_GNU_STACK, PF_X) == NULL)
    return R_BINFMT_NX_ENABLED;
  return R_BINFMT_NX_DISABLED;
}


static r_binfmt_relro_e r_binfmt_elf64_check_relro(r_binfmt_s *bin, r_binfmt_elf64_s *elf) {
  Elf64_Word i;
  Elf64_Sword d_tag;

  R_BINFMT_ASSERT_RET(R_BINFMT_RELRO_UNKNOWN, elf->ehdr != NULL);
  R_BINFMT_ASSERT_RET(R_BINFMT_RELRO_UNKNOWN, elf->phdr != NULL);
  R_BINFMT_ASSERT_RET(R_BINFMT_RELRO_UNKNOWN, elf->dyntab != NULL);

  if(r_binfmt_elf64_get_segment(bin, elf, PT_GNU_RELRO, ~0) == NULL) {
    return R_BINFMT_RELRO_DISABLED;
  }

  for(i = 0; i < elf->dyntab_entries; i++) {
    R_BINFMT_GET_INT(d_tag, elf->dyntab[i].d_tag, bin->endian);

    if(d_tag == DT_BIND_NOW)
      return R_BINFMT_RELRO_FULL;
  }

  return R_BINFMT_RELRO_PARTIAL;
}


static r_binfmt_rpath_e r_binfmt_elf64_check_rpath(r_binfmt_s *bin, r_binfmt_elf64_s *elf) {
  Elf64_Word i;
  Elf64_Sword d_tag;

  R_BINFMT_ASSERT_RET(R_BINFMT_RPATH_UNKNOWN, elf->dyntab != NULL);

  for(i = 0; i < elf->dyntab_entries; i++) {
    R_BINFMT_GET_INT(d_tag, elf->dyntab[i].d_tag, bin->endian);

    if(d_tag == DT_RPATH)
      return R_BINFMT_RPATH_ENABLED;
  }

  return R_BINFMT_RPATH_DISABLED;
}

static r_binfmt_rpath_e r_binfmt_elf64_check_runpath(r_binfmt_s *bin, r_binfmt_elf64_s *elf) {
  Elf64_Word i;
  Elf64_Sword d_tag;

  R_BINFMT_ASSERT_RET(R_BINFMT_RUNPATH_UNKNOWN, elf->dyntab != NULL);

  for(i = 0; i < elf->dyntab_entries; i++) {
    R_BINFMT_GET_INT(d_tag, elf->dyntab[i].d_tag, bin->endian);

    if(d_tag == DT_RUNPATH)
      return R_BINFMT_RUNPATH_ENABLED;
  }

  return R_BINFMT_RUNPATH_DISABLED;
}

static r_binfmt_pie_e r_binfmt_elf64_check_pie(r_binfmt_s *bin, r_binfmt_elf64_s *elf) {
  Elf64_Half e_type;

  R_BINFMT_ASSERT_RET(R_BINFMT_PIE_UNKNOWN, elf->ehdr != NULL);

  R_BINFMT_GET_INT(e_type, elf->ehdr->e_type, bin->endian);

  if(e_type != ET_DYN)
    return R_BINFMT_PIE_DISABLED;

  return R_BINFMT_PIE_ENABLED;
}

/* Fill the BINFMT structure if it's a correct ELF64 */
r_binfmt_err_e r_binfmt_elf64_load(r_binfmt_s *bin) {
  r_binfmt_elf64_s elf;

  memset(&elf, 0, sizeof(elf));

  if((bin->type = r_binfmt_elf_type(bin)) != R_BINFMT_TYPE_ELF64)
    return R_BINFMT_ERR_UNRECOGNIZED;

  bin->endian = r_binfmt_elf64_getendian(bin);

  if(bin->endian == R_BINFMT_ENDIAN_UNDEF)
    return R_BINFMT_ERR_NOTSUPPORTED;

  r_binfmt_elf64_load_ehdr(bin, &elf);
  r_binfmt_elf64_load_phdr(bin, &elf);
  r_binfmt_elf64_load_shdr(bin, &elf);
  r_binfmt_elf64_load_symtab(bin, &elf);
  r_binfmt_elf64_load_dynsym(bin, &elf);
  r_binfmt_elf64_load_dyntab(bin, &elf);

  r_binfmt_elf64_load_segments(bin, &elf);
  r_binfmt_elf64_load_sections(bin, &elf);
  r_binfmt_elf64_load_syms(bin, &elf);

  bin->arch = r_binfmt_elf64_getarch(bin, &elf);
  bin->entry = r_binfmt_elf64_getentry(bin, &elf);
  bin->elf.nx = r_binfmt_elf64_check_nx(bin, &elf);
  bin->elf.ssp = r_binfmt_elf_check_ssp(bin);
  bin->elf.rpath = r_binfmt_elf64_check_rpath(bin, &elf);
  bin->elf.runpath = r_binfmt_elf64_check_runpath(bin, &elf);
  bin->elf.relro = r_binfmt_elf64_check_relro(bin, &elf);
  bin->elf.pie = r_binfmt_elf64_check_pie(bin, &elf);

  return R_BINFMT_ERR_OK;
}
