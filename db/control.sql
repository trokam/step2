-- Database generated with pgModeler (PostgreSQL Database Modeler).
-- pgModeler  version: 0.8.1
-- PostgreSQL version: 9.4
-- Project Site: pgmodeler.com.br
-- Model Author: ---


-- Database creation must be done outside an multicommand file.
-- These commands were put in this file only for convenience.
-- -- object: new_database | type: DATABASE --
-- -- DROP DATABASE IF EXISTS new_database;
-- CREATE DATABASE new_database
-- ;
-- -- ddl-end --
-- 

-- object: public.command | type: TABLE --
-- DROP TABLE IF EXISTS public.command CASCADE;
CREATE TABLE public.command(
	run bool NOT NULL,
	active bool NOT NULL
);
-- ddl-end --
ALTER TABLE public.command OWNER TO postgres;
-- ddl-end --


